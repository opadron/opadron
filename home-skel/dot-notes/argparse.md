
    from argparse import ArgumentParser
    arg_parser = ArgumentParser()
    aa = arg_parser.add_argument

    aa('positional', nargs=1, help='positional argument')

    aa(
        '-f',
        '--flag-argument',
        action='store_true',
        default=False,
        help='flag argument',
    )

    aa(
        '-v',
        '--value-argument',
        type=int,
        nargs=1,
        default=42,
        help='value argument',
    )

    aa(
        '-m',
        '--multi-argument',
        type=float,
        nargs='*' or '+',
        default=[1.0, 2.0, 3.0],
        help='mutli argument',
    )

    args = arg_parser.parse_args()

