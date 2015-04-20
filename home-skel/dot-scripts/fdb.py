
import argparse
import datetime
import hashlib
import os
import os.path
import re
import sys

import sqlite3 as sql
import itertools as it

from contextlib import contextmanager

R_COMMENT = re.compile(r'''^\s*#''')
R_BLANK_LINE = re.compile(r'''^\s*$''')

R_SPLIT_CODE = re.compile(r'''^([a-zA-Z][a-zA-Z0-9]{2}) - (.+)$''')

R_TWO_DASH_LINE = re.compile(r'''^([a-zA-Z][a-zA-Z0-9 ]{5}) - (.+)$''')

R_INCLUDE_LINE = re.compile(r'''^#include "(.+)"$''')

R_SPLIT = re.compile(
    '^' + ''.join(
        ('({X}{{' + str(x) + '}})')
        for x in (4, 6, 40, 7, 7, 10, 6)
    ).format(X='.')
)

TABLE  = 'CREATE TABLE IF NOT EXISTS'
STRING = 'VARCHAR(1024)'
PKEY   = 'PRIMARY KEY'
FKEY   = 'FOREIGN KEY'
CHAR   = 'CHAR(6)'
NNULL  = 'NOT NULL'
REF    = 'REFERENCES'
INT    = 'INTEGER'
DATE   = 'TIMESTAMP'
NOW    = 'CURRENT_TIMESTAMP'

DDD = dict(
    TABLE  = TABLE ,
    STRING = STRING,
    PKEY   = PKEY  ,
    FKEY   = FKEY  ,
    CHAR   = CHAR  ,
    NNULL  = NNULL ,
    REF    = REF   ,
    INT    = INT   ,
    DATE   = DATE  ,
    NOW    = NOW   ,
)

tmp_tables_create = set()

_time_spec_parser = None
def time_spec_parser():
    global _time_spec_parser
    if _time_spec_parser is None:
        _time_spec_parser = argparse.ArgumentParser()
        _time_spec_parser.add_argument('-d', '--day'  , nargs='?', default='')
        _time_spec_parser.add_argument('-m', '--month', nargs='?', default='')
        _time_spec_parser.add_argument('-y', '--year' , nargs='?', default='')

    return _time_spec_parser

def parse_time_spec(args=None):
    if args is None:
        args = sys.argv[1:]

    if isinstance(args, list):
        args = time_spec_parser().parse_args(args)

    now = datetime.datetime.now()

    args.year = (     now.year       if args.year is None
                 else int(args.year) if args.year
                 else None                               )

    args.month = (     now.month       if args.month is None
                  else int(args.month) if args.month
                  else None                                 )

    args.day = (    now.day        if args.day is None
                else int(args.day) if args.day 
                else None                              )

    xor = lambda a, b: (a and (not b)) or ((not a) and b)

    f = False
    b = bool(args.day  ) ; f = f or b ; args.day   = ( now.day   if f and not b else args.day   )
    b = bool(args.month) ; f = f or b ; args.month = ( now.month if f and not b else args.month )
    b = bool(args.year ) ; f = f or b ; args.year  = ( now.year  if f and not b else args.year  )

    return args

def gen_time_spec_query_fragment(args, head=False):
    query = ''
    query_args = []
    if args.year:
        if head:
            query += ' WHERE'
        query += ' ( STRFTIME(\'%Y\', date) = ?'
        query_args.append(str(args.year))

        if args.month:
            query += ' AND STRFTIME(\'%m\', date) = ?'
            query_args.append('%02d' % args.month)

            if args.day:
                query += ' AND STRFTIME(\'%d\', date) = ?'
                query_args.append('%02d' % args.day)

        query += ' )'

    return query, query_args

@contextmanager
def serial_connection(path):
    conn = sql.connect(path, timeout=1.0, isolation_level='EXCLUSIVE')

    @contextmanager
    def transaction_manager(*args, **kwds):
        cursor = conn.cursor(*args, **kwds)
        cursor.execute('BEGIN TRANSACTION')
        try:
            yield cursor
        except Exception:
            conn.rollback()
            raise
        else:
            try: conn.commit()
            except sql.OperationalError: pass
        finally:
            cursor.close()

    try:
        yield transaction_manager
    finally:
        conn.close()

def remove_leading_zeros(x):
    return ''.join(it.dropwhile(lambda y: y == '0', x))

class Account(object):
    def __init__(self, namespace, path):
        i = path.rfind('/')
        a, b = path[:i], path[i+1:]

        self.name = b
        self.parent = None
        if a:
            if a not in namespace:
                Account(namespace, a)

            self.parent = namespace[a]
        self.code = None
        self.path = path
        namespace[path] = self

    @staticmethod
    def get(namespace, key):
        return (
            namespace.get(key) or Account(namespace, key)
        )

class Transaction(object):
    def __init__(self, datestring, description):
        month, day, year = tuple(
            int(remove_leading_zeros(x) or '0')
            for x in ( datestring[ :2],
                       datestring[2:4],
                       datestring[4: ] )
        )

        if year < 1900:
            year += 2000

        self.date = datetime.date(year, month, day)
        self.description = description
        self.splits = []

class Split(object):
    def __init__(self, cls, src, dst, amount, com):
        self.src = src
        self.dst = dst

        a, b = amount.split('.')
        a = int(remove_leading_zeros(a) or 0)

        ldigits = len(str(a)) if a != 0 else 0
        rdigits = len(b)

        for i in range(rdigits):
            a *= 10

        a += int(remove_leading_zeros(b) or 0)

        self.amount = a
        self.ldigits = ldigits
        self.rdigits = rdigits
        self.tags = set()
        self.meta = set()
        self.cls = cls
        self.com = com

def iter_lines(ledger_path, visited_set=None):
    if visited_set is None:
        visited_set = set()

    if ledger_path in visited_set:
        return

    visited_set.add(ledger_path)

    with open(ledger_path) as f:
        for line in f:
            line = line.rstrip()
            M = R_INCLUDE_LINE.match(line)
            if M is not None:
                target_path = M.group(1)
                target_path = os.path.normpath(
                    os.path.join(
                        os.path.dirname(ledger_path),
                        target_path
                    )
                )

                for sub_line in iter_lines(target_path, visited_set):
                    yield sub_line

            if R_COMMENT.match(line) is not None: continue
            if R_BLANK_LINE.match(line) is not None: continue

            yield line

def compute_path_hash(ledger_path):
    result = hashlib.md5(os.path.abspath(ledger_path)).hexdigest()
    return result

def compute_checksum_hash(ledger_path):
    result = hashlib.md5(''.join(iter_lines(ledger_path))).hexdigest()
    return result

def ingest(ledger_path):
    code_to_account_path = dict()
    split_codes = dict()
    accounts = dict()
    transactions = []
    commodities = dict()

    last_transaction = None
    last_split = None
    last_cls = None

    for line in iter_lines(ledger_path):
        M = R_SPLIT_CODE.match(line)
        if M is not None: # split code
            key, value = M.group(1).strip(), M.group(2).strip()
            split_codes[key] = value

            continue

        M = R_TWO_DASH_LINE.match(line)
        if M is not None: # account code/commodity code
            key, value = M.group(1).strip(), M.group(2).strip()
            is_account = value.startswith('/')

            if is_account:
                code_to_account_path[key] = value
                Account.get(accounts, value).code = key

            else:
                assert key not in commodities
                commodities[key] = value

            continue

        # if we get here, it must be a transaction/split line
        line, meta = line[:80], line[80:].strip()
        M = R_SPLIT.match(line)

        cls, date, desc, src, dst, amt, com = tuple(
            M.group(i+1).strip() for i in range(7)
        )

        if desc: # new transaction
            last_transaction = Transaction(date, desc)
            transactions.append(last_transaction)

        if ( src and dst and amt and com ): # new split
            last_cls = cls or last_cls

            if src in code_to_account_path:
                src = code_to_account_path[src]
            else:
                assert src.startswith('/')
                code_to_account_path[src] = src

            if dst in code_to_account_path:
                dst = code_to_account_path[dst]
            else:
                assert dst.startswith('/')
                code_to_account_path[dst] = dst

            src = Account.get(accounts, src)
            dst = Account.get(accounts, dst)

            last_split = Split(last_cls, src, dst, amt, com)
            last_transaction.splits.append(last_split)

            if meta:
                key, val = meta.split(' ', 1)
                key = key.strip()
                val = val.strip()

                if key == 'tag':
                    last_split.tags.add(val)
                else:
                    last_split.meta.add((key, val))

    return accounts, transactions, commodities, split_codes, dict(
        (v, k) for (k, v) in code_to_account_path.items()
    )

def amount_display(amt, ldig, rdig, maxL, maxR,
                   mod=3, mod_sep=',', frac_sep='.', plus=' '):

    negative = (amt < 0)
    if negative: amt = -amt

    if amt == 0:
        return
        ''.join((
            ' '*maxL,
            '-' if negative else plus if amt > 0 else ' ',
            frac_sep.join(('0', '0'*rdig)),
            ' '*(maxR - rdig)
        ))

    amt_disp = str(amt)
    amt_disp = ('0'*max(maxR - len(amt_disp), 0)) + amt_disp
    whole, frac = (amt_disp[:-maxR], amt_disp[-maxR:-(maxR-rdig) or None])

    if not whole:
        whole = '0'

    n = len(whole)
    nm = (mod - (n % mod)) % mod
    whole = mod_sep.join(
        whole[ max(mod*i - nm, 0) : mod*(i+1) - nm ]
        for i in range((n + mod - 1)//mod)
    )

    if rdig > 0:
        amt_disp = frac_sep.join((whole, frac))

    return ''.join((
        ' '*(maxL - n - len(mod_sep)*((n-1)//mod - 1)),
        '-' if negative else plus if amt > 0 else ' ',
        amt_disp,
        ' '*(maxR - rdig)
    ))

def create_ledger_temp(cur, args):
    ret = 'ledger_temp'
    if ret in tmp_tables_create: return ret

    cur.execute('''
        CREATE TEMPORARY TABLE {ret}
        ( date         TIMESTAMP,
          tran         VARCHAR(1024),
          src          VARCHAR(1024),
          dst          VARCHAR(1024),
          amt          INTEGER,
          ldig         INTEGER,
          rdig         INTEGER,
          maxL         INTEGER,
          maxR         INTEGER,
          amt_disp     VARCHAR(1024),
          com          VARCHAR(1024) );
    '''.format(ret=ret))

    cur.execute('SELECT MAX(ldig) FROM ledger_view')
    maxL = cur.fetchone()[0]

    cur.execute('SELECT MAX(rdig) FROM ledger_view')
    maxR = cur.fetchone()[0]

    query = ('''
        SELECT tid, date, desc, src ,
               dst, amt , ldig, rdig,
               com
        FROM ledger_view
    ''')

    q, a = gen_time_spec_query_fragment(args, True)

    query += q

    cur.execute(query, a)

    rows = []

    last_tid = None
    last_src = None

    while True:
        row = cur.fetchone()
        if row is None: break

        tid, date, desc, src, dst, amt, ldig, rdig, com = row

        if tid == last_tid:
            tid = None
            date = None
            desc = None

            if src == last_src:
                src = None
            else:
                last_src = src

        else:
            last_tid = tid
            last_src = src

        for i in range(rdig, maxR):
            amt *= 10

        amt_disp = amount_display(amt, ldig, rdig, maxL, maxR)

        rows.append((
            date, desc, src, dst, amt, ldig, rdig, maxL, maxR, amt_disp, com
        ))

    cur.executemany(
        'INSERT INTO {ret} VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)'.format(
            ret=ret
        ),
        rows
    )

    tmp_tables_create.add(ret)

def fa(x, l, r, L, R):
    return amount_display(x, l, r, L, R, plus='+') if x else ''

def create_bsheet_temp(cur, account, args):
    ret = 'bsheet'

    cur.execute('SELECT code FROM commodities')
    commodities = [ x[0] for x in cur ]
    equity = dict((x, 0) for x in commodities)

    if account is not None:
        cur.execute('''
            SELECT code, path FROM accounts
            WHERE code = ? OR path = ?
        ''', (account, account))

        account_code, account_path = cur.fetchone()

        ret += '_' + account_code

    if ret not in tmp_tables_create:
        cur.execute('SELECT MAX(ldig) FROM ledger_view')
        maxL = cur.fetchone()[0]

        cur.execute('SELECT MAX(rdig) FROM ledger_view')
        maxR = cur.fetchone()[0]

        def check_account_clause(path):
            return '''
                ( ( CASE WHEN spath LIKE '{path}%' THEN -amt ELSE 0 END )
                + ( CASE WHEN dpath LIKE '{path}%' THEN  amt ELSE 0 END ) )
            '''.format(path=path)

        if account is None:
            cur.execute('''
                CREATE TEMPORARY TABLE {ret} (
                    "date"       TIMESTAMP,
                    "desc"       VARCHAR(1024),
                    "src"        VARCHAR(1024),
                    "dst"        VARCHAR(1024),
                    "A"          VARCHAR(1024),
                    "L"          VARCHAR(1024),
                    "I"          VARCHAR(1024),
                    "E"          VARCHAR(1024),
                    "Q"          VARCHAR(1024),
                    "commodity"  VARCHAR(1024),
                    {balances}
                )
            '''.format(
                ret=ret,
                balances=',\n'.join(
                    'q_' + x + ' VARCHAR(1024)'
                    for x in commodities
                ),
            ))

            query = ('''
                SELECT
                    tid, date, desc, src, dst,

                    {formatA} AS AA,
                    {formatL} AS LL,
                    {formatI} AS II,
                    {formatE} AS EE,
                    {formatQ} AS QQ,

                    ldig, rdig, com

                FROM ledger_view

                WHERE (
                       spath LIKE '/A%'
                    OR spath LIKE '/L%'
                    OR spath LIKE '/I%'
                    OR spath LIKE '/E%'
                    OR spath LIKE '/Q%'

                    OR dpath LIKE '/A%'
                    OR dpath LIKE '/L%'
                    OR dpath LIKE '/I%'
                    OR dpath LIKE '/E%'
                    OR dpath LIKE '/Q%'
                )
            '''.format(
                formatA=check_account_clause('/A'),
                formatL=check_account_clause('/L'),
                formatI=check_account_clause('/I'),
                formatE=check_account_clause('/E'),
                formatQ=check_account_clause('/Q'),
            ))

            q, a = gen_time_spec_query_fragment(args)
            if q:
                query += ' AND ' + q

            cur.execute(query, a)

            rows = []
            tids = []

            for (
                tid, date, desc, src, dst, aa, ll, ii, ee, qq, ldig, rdig, com
            ) in reversed(
                cur.fetchall()
            ):
                for i in range(rdig, maxR):
                    aa *= 10
                    ll *= 10
                    ii *= 10
                    ee *= 10
                    qq *= 10

                equity[com] += (aa + ll)

                rows.append([
                    date, desc, src, dst,
                    fa(aa, ldig, rdig, maxL, maxR),
                    fa(ll, ldig, rdig, maxL, maxR),
                    fa(ii, ldig, rdig, maxL, maxR),
                    fa(ee, ldig, rdig, maxL, maxR),
                    fa(qq, ldig, rdig, maxL, maxR),
                    com,
                ] + [
                    amount_display(
                        equity[x],
                        len(str(abs(equity[x]))) - maxR, maxR,
                        maxL, maxR
                    )
                    for x in commodities
                ])

                tids.append(tid)

            rows = list(reversed(rows))
            tids = list(reversed(tids))

            last_tid = None
            last_src = None

            for (i,row) in enumerate(rows):
                tid = tids[i]
                date, desc, src = row[:3]
    
                if tid == last_tid:
                    tid = None
                    date = None
                    desc = None

                    if src == last_src:
                        src = None
                    else:
                        last_src = src

                else:
                    last_tid = tid
                    last_src = src

                row[:3] = (date, desc, src)


            cur.executemany('''
                INSERT INTO {ret} VALUES({QM})
            '''.format(
                ret=ret,
                QM=', '.join('?' for x in rows[0])
            ), rows)


        else:
            cur.execute('''
                CREATE TEMPORARY TABLE {ret} (
                    "date"      TIMESTAMP,
                    "desc"      VARCHAR(1024),
                    "src"       VARCHAR(1024),
                    "dst"       VARCHAR(1024),
                    "debit"     VARCHAR(1024),
                    "credit"    VARCHAR(1024),
                    "commodity" VARCHAR(1024),
                    {balances} 
                )
            '''.format(
                ret=ret,
                balances=',\n'.join(
                    'q_' + x + ' VARCHAR(1024)'
                    for x in commodities
                ),
            ))

            query = ('''
                SELECT
                    tid, date, desc, src, dst,
                    {format} AS delta,
                    ldig, rdig, com

                FROM ledger_view

                WHERE (
                        spath LIKE '{account}%'
                    OR  dpath LIKE '{account}%'
                )
            '''.format(
                account=account_path,
                format=check_account_clause(account_path),
            ))

            q, a = gen_time_spec_query_fragment(args)
            if q:
                query += ' AND ' + q

            cur.execute(query, a)

            rows = []
            tids = []

            for (
                tid, date, desc, src, dst, delta, ldig, rdig, com
            ) in reversed(
                cur.fetchall()
            ):

                for i in range(rdig, maxR):
                    delta *= 10

                equity[com] += delta

                rows.append([
                    date, desc, src, dst,
                    fa(-max(-delta, 0), ldig, rdig, maxL, maxR),
                    fa( max( delta, 0), ldig, rdig, maxL, maxR),
                    com,
                ] + [
                    amount_display(
                        equity[x],
                        len(str(abs(equity[x]))) - maxR, maxR,
                        maxL, maxR
                    )
                    for x in commodities
                ])

                tids.append(tid)


            rows = list(reversed(rows))
            tids = list(reversed(tids))

            last_tid = None
            last_src = None

            for (i,row) in enumerate(rows):
                tid = tids[i]
                date, desc, src = row[:3]
    
                if tid == last_tid:
                    tid = None
                    date = None
                    desc = None

                    if src == last_src:
                        src = None
                    else:
                        last_src = src

                else:
                    last_tid = tid
                    last_src = src

                row[:3] = (date, desc, src)


            cur.executemany('''
                INSERT INTO {ret} VALUES({QM})
            '''.format(
                ret=ret,
                QM=', '.join('?' for x in rows[0])
            ), rows)

        tmp_tables_create.add(ret)

        if account is None:
            return ret, (
                'Date', 'Transaction', 'From', 'To',
                '[A]', '[L]', '[I]', '[E]',
                'Prior Equity', 'Commodity',
            ) + tuple('B(' + x + ')' for x in commodities)
        else:
            return ret, tuple(
                'Date Transaction From To Debit Credit Commodity'.split()
            ) + tuple('B(' + x + ')' for x in commodities)

def create_summary_temp(cur, args):
    ret = 'summary'

    cur.execute('SELECT code FROM commodities')
    commodities = [ x[0] for x in cur ]
    com_index = dict((x, i) for (i, x) in enumerate(commodities))

    credits = dict()
    debits = dict()

    if ret not in tmp_tables_create:
        cur.execute('SELECT MAX(ldig) FROM ledger_view')
        maxL = cur.fetchone()[0]

        cur.execute('SELECT MAX(rdig) FROM ledger_view')
        maxR = cur.fetchone()[0]

        cur.execute('''
            CREATE TEMPORARY TABLE {ret} (
                path    VARCHAR(1024),
                account VARCHAR(1024),
                {debits}, {credits}, {balances}
            )
        '''.format(
            ret=ret,
            debits=',\n'.join(
                'd_' + x + ' VARCHAR(1024)'
                for x in commodities
            ),
            credits=',\n'.join(
                'c_' + x + ' VARCHAR(1024)'
                for x in commodities
            ),
            balances=',\n'.join(
                'b_' + x + ' VARCHAR(1024)'
                for x in commodities
            ),
        ))

        query = ('''
            SELECT date, spath, dpath, amt, ldig, rdig, com
            FROM ledger_view
        ''')

        q, a = gen_time_spec_query_fragment(args, True)
        if q:
            query += q

        cur.execute(query, a)

        def debit(path, amt, com):
            if path not in debits:
                debits[path] = [0 for x in commodities]

            debits[path][com_index[com]] += amt

            parent, _ = path.rsplit('/', 1)
            if parent: debit(parent, amt, com)

        def credit(path, amt, com):
            if path not in credits:
                credits[path] = [0 for x in commodities]

            credits[path][com_index[com]] += amt

            parent, _ = path.rsplit('/', 1)
            if parent: credit(parent, amt, com)

        for date, spath, dpath, amt, ldig, rdig, com in cur:
            for i in range(rdig, maxR):
                amt *= 10

            debit(spath, amt, com)
            credit(dpath, amt, com)

        rows = []

        for path in sorted(set(iter(debits)) | set(iter(credits))):
            nlevels = len(path[1:].split('/'))
            debit = debits.get(path)
            credit = credits.get(path)
            _, short_name = path.rsplit('/', 1)

            if debit is None: debit = [0 for x in commodities]
            if credit is None: credit = [0 for x in commodities]

            debits[path] = debit
            credits[path] = credit

            rows.append((
                path,
                ('| '*(nlevels - 1)) +
                ('+-' if nlevels > 1 else '') +
                short_name,
            ) + tuple(
                fa(-x, ldig, rdig, maxL, maxR)
                for x in debit
            ) + tuple(
                fa(x, ldig, rdig, maxL, maxR)
                for x in credit
            ) + tuple(
                amount_display(
                    a - b, ldig, rdig, maxL, maxR
                )
                for (a,b) in zip(credit, debit)
            ))

        AD = debits.get('/A')
        AC = credits.get('/A')

        LD = debits.get('/L')
        LC = credits.get('/L')

        if AD is None: AD = [0 for x in commodities]
        if AC is None: AC = [0 for x in commodities]

        if LD is None: LD = [0 for x in commodities]
        if LC is None: LC = [0 for x in commodities]

        debits['/A'] = AD
        credits['/A'] = AC

        debits['/L'] = LD
        credits['/L'] = LC

        cur.executemany('''
            INSERT INTO {ret} VALUES({QM})
        '''.format(
            ret=ret,
            QM=', '.join('?' for x in rows[0])
        ), rows)

        cur.execute('''
            INSERT INTO {ret} VALUES({QM})
        '''.format(
            ret=ret,
            QM=', '.join('?' for x in rows[0])
        ),
            ( None, )*len(rows[0])
        )

        cur.execute('''
            INSERT INTO {ret} VALUES({QM})
        '''.format(
            ret=ret,
            QM=', '.join('?' for x in rows[0])
        ), (
            None, 'TOTAL EQUITY'
        ) + tuple(
            fa(-(ad + lc), maxL, maxR, maxL, maxR)
            for (ad, lc) in zip(AD, LC)
        ) + tuple(
            fa((ac + ld), maxL, maxR, maxL, maxR)
            for (ac, ld) in zip(AC, LD)
        ) + tuple(
            amount_display(
                (ac + ld) - (ad + lc), maxL, maxR, maxL, maxR
            )
            for (ad, ac, ld, lc) in zip(AD, AC, LD, LC)
        ))

        tmp_tables_create.add(ret)

        return ret, (
            'Account',
        ) + tuple(
            'D(' + x + ')' for x in commodities
        ) + tuple(
            'C(' + x + ')' for x in commodities
        ) + tuple(
            'B(' + x + ')' for x in commodities
        )

def ensure_tables(cur):
    cur.execute('''
        {TABLE} commodities ( code {CHAR}   {PKEY}  ,
                              desc {STRING} {NNULL} );
    '''.format(**DDD))

    cur.execute('''
        {TABLE} accounts ( code   {CHAR}   {PKEY}        ,
                           parent {CHAR}                 ,
                           path   {STRING} {NNULL} UNIQUE,
                           name   {STRING} {NNULL}       ,

                           {FKEY}(parent)
                           {REF} accounts(code)
                           MATCH SIMPLE                  );
    '''.format(**DDD))


    cur.execute('''
        {TABLE} transactions ( id   {INT}    {PKEY}       ,
                               date {DATE}   DEFAULT {NOW},
                               desc {STRING} {NNULL}      );
    '''.format(**DDD))

    cur.execute('''
        {TABLE} split_codes ( code {CHAR}   {PKEY},
                              desc {STRING} {NNULL} );
    '''.format(**DDD))

    cur.execute('''
        {TABLE} splits ( id   {INT}  {PKEY} ,
                         cls  {CHAR}        ,
                         tid  {INT}  {NNULL},
                         src  {CHAR} {NNULL},
                         dst  {CHAR} {NNULL},
                         amt  {INT}  {NNULL},
                         ldig {INT}  {NNULL},
                         rdig {INT}  {NNULL},
                         com  {CHAR} {NNULL},

                        {FKEY}(tid) {REF} transactions(id  ),
                        {FKEY}(cls) {REF} split_codes (code),
                        {FKEY}(src) {REF} accounts    (code),
                        {FKEY}(dst) {REF} accounts    (code),
                        {FKEY}(com) {REF} commodities (code) );
    '''.format(**DDD))

    cur.execute('''
        {TABLE} split_meta_data ( id  {INT}    {PKEY}                ,
                                  sid {INT}    {NNULL}               ,
                                  key {STRING} {NNULL} DEFAULT 'memo',
                                  val {STRING} {NNULL}               ,

                                  {FKEY}(sid) {REF} splits(id)       );
    '''.format(**DDD))

    cur.execute('''
        {TABLE} tags ( id    {INT}    {PKEY}        ,
                       label {STRING} {NNULL} UNIQUE);
    '''.format(**DDD))

    cur.execute('''
        {TABLE} split_tags ( id  {INT} {PKEY} ,
                             sid {INT} {NNULL},
                             tid {INT} {NNULL},

                             {FKEY}(sid) {REF} splits(id),
                             {FKEY}(tid) {REF} tags  (id));
    '''.format(**DDD))

def ensure_views(cur):
   cur.execute('''
        CREATE VIEW ledger_view AS

        SELECT
            TS.id   AS tid,
            TS.date AS date,
            TS.desc AS desc,
            TS.src  AS src,
            SA.path AS spath,
            TS.dst  AS dst,
            DA.path AS dpath,
            TS.amt  AS amt,
            TS.ldig AS ldig,
            TS.rdig AS rdig,
            C.code  AS com,
            C.desc  AS cdesc

        FROM
                 {TSPLITS}      AS TS
            JOIN {ACCOUNTS}     AS SA ON TS.src = SA.code
            JOIN {ACCOUNTS}     AS DA ON TS.dst = DA.code
            JOIN {COMMODITIES}  AS C  ON TS.com = C.code

        ORDER BY TS.date DESC, TS.id, TS.sid

    '''.format(
        TSPLITS='''(
            SELECT A.id   AS id  , A.date AS date, A.desc AS desc,
                   B.src  AS src , B.dst  AS dst , B.amt  AS amt ,
                   B.ldig AS ldig, B.rdig AS rdig, B.com  AS com , B.id AS sid
            FROM transactions AS A JOIN splits AS B ON A.id = B.tid
            ORDER BY A.date DESC, A.id, B.id
        )''',

        ACCOUNTS='accounts',

        COMMODITIES='commodities',
    ))

def ensure_dir(dir_path):
    if not os.path.isdir(dir_path):
        if os.path.exists(dir_path):
            os.remove(dir_path)

        os.mkdir(dir_path)

def format_rows(cur, headers=None):
    nColumns = len(headers)
    rows = list(iter(cur))
    maxLengths = [len(header) for header in headers]
    lengths = [None for x in headers]

    for (i, row) in enumerate(rows):
        rows[i]    = [ str(column)
                           if column is not None
                           else ''
                       for column in rows[i] ]
        lengths[:] = map(len, rows[i])

        maxLengths[:] = [ max(a, b) for (a,b) in zip(maxLengths, lengths) ]

    return '\n'.join((
        ' '.join(
            (
                '%*s%s%*s' % (
                    (length - len(header))//2, '',
                    header,
                    (length - len(header) - (length - len(header))//2),
                    '',
                )
            ) for (length,header) in zip(maxLengths,headers)
        ),
        ' '.join(
            '-'*length for length in maxLengths
        ),
        '\n'.join(
            ' '.join(
                ('%-*s' % (length, column))
                for (length,column) in zip(maxLengths, row)
            ) for row in rows
        )
    ))

def cache_check(ledger_path, ledger_checksum_path, ledger_db_path):
    old_hash = None
    new_hash = None
    result = os.path.isfile(ledger_checksum_path)
    if not result: return ( result, new_hash )

    result = os.path.isfile(ledger_db_path)
    if not result: return ( result, new_hash )

    new_hash = compute_checksum_hash(ledger_path)
    result = ( new_hash == old_hash )

    return ( result, new_hash )

@contextmanager
def connect(ledger_path):
    from os import path
    fdb_dir = path.expanduser(path.join('~', '.fdb'))
    checksum_dir = path.join(fdb_dir, 'checksums')
    db_dir = path.join(fdb_dir, 'dbs')

    ensure_dir(fdb_dir)
    ensure_dir(checksum_dir)
    ensure_dir(db_dir)

    ledger_path_hash = compute_path_hash(ledger_path)
    ledger_checksum_path = path.join(checksum_dir, ledger_path_hash)
    ledger_db_path = path.join(db_dir, ledger_path_hash)

    cache_hit, hash = cache_check(
        ledger_path,
        ledger_checksum_path,
        ledger_db_path
    )

    if cache_hit:
        with serial_connection(ledger_db_path) as conn:
            yield conn

    else:
        if hash is None:
            hash = compute_checksum_hash(ledger_path)

        try: os.remove(ledger_checksum_path)
        except OSError: pass

        try: os.remove(ledger_db_path)
        except OSError: pass

        A, T, C, S, K = ingest(ledger_path)

        with serial_connection(ledger_db_path) as conn:
            with conn() as cur:
                ensure_tables(cur)
                ensure_views(cur)

                counter = 0
                for path in sorted(A.keys()):
                    account = A[path]
                    code = K.get(path)

                    if code is None:
                        code = 'unknown_%04d' % counter
                        counter += 1

                    parent = None
                    if account.parent:
                        parent = account.parent.path
                        parent = K.get(parent)
                        if parent is None:
                            parent = 'unknown_%04d' % counter
                            counter += 1

                    name = account.name

                    if parent is None:
                        cur.execute('''
                            INSERT INTO accounts(code, path, name)
                            VALUES(?, ?, ?)
                        ''', (code, path, name))
                    else:
                        cur.execute('''
                            INSERT INTO accounts(code, parent, path, name)
                            VALUES(?, ?, ?, ?)
                        ''', (code, parent, path, name))

                all_tags = set()
                for transaction in T:
                    for split in transaction.splits:
                        all_tags |= split.tags

                cur.executemany('''
                    INSERT INTO commodities(code, desc) VALUES(?, ?)
                ''', C.items())

                cur.executemany('''
                    INSERT INTO split_codes(code, desc) VALUES(?, ?)
                ''', S.items())

                cur.executemany('''
                    INSERT INTO tags(label) VALUES(?)
                ''', ((x,) for x in all_tags))

                cur.execute('SELECT label, id FROM tags')
                tag_to_id = dict(cur.fetchall())

                for transaction in T:
                    cur.execute('''
                        INSERT INTO transactions(date, desc)
                        VALUES(?, ?)
                    ''', (transaction.date, transaction.description))

                    cur.execute('SELECT last_insert_rowid()')
                    trans_id = cur.fetchone()[0]

                    for split in transaction.splits:
                        cur.execute('''
                            INSERT INTO splits(
                                cls, tid, src, dst, amt, ldig, rdig, com
                            ) VALUES(?, ?, ?, ?, ?, ?, ?, ?)
                        ''',
                            (
                                split.cls,
                                trans_id,
                                K[split.src.path],
                                K[split.dst.path],
                                split.amount,
                                split.ldigits,
                                split.rdigits,
                                split.com
                            )
                        )

                        cur.execute('SELECT last_insert_rowid()')
                        split_id = cur.fetchone()[0]

                        cur.executemany('''
                            INSERT INTO split_tags(sid, tid)
                            VALUES(?, ?)
                        ''', ((split_id, tag_to_id[tag]) for tag in split.tags))

                        cur.executemany('''
                            INSERT INTO split_meta_data(sid, key, val)
                            VALUES(?, ?, ?)
                        ''', ((split_id, key, value)
                              for (key, value) in split.meta))

            with open(ledger_checksum_path, 'w') as f:
                f.write(hash)
                f.flush()

            yield conn

