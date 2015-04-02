
install GnuPG

    - Ubuntu/Debian

    ```
    $ apt-get install gpg
    ```

    - Fedora/Redhat

    ```
    $ yum install gpg
    ```

    - Arch

    ```
    $ pacman -Sy gnupg
    ```

    - Max OS X (native)
        Download the [package](https://gpgtools.org)

    - Mac OS X (using [homebrew](https://brew.sh))

    ```
    $ brew install gpg
    ```

    - Windows (other)

        Visit the gnupg [downloads](https://www.gnupg.org/download) page for
        source and binary distributions.





gpg --gen-key

Key type: RSA and RSA
Key size: 2048 bits
Expiration: never expires
Real name:
email:
comment [github username]
pass phrase: Use a complex passphrase and dedicate it to memory.

gpg --armor -u user@email.com --sign signed-cla-user

gpg --armor --export user@email.com > user.gpg

[send signed-cla-user.asc and user.gpg to project]


git config user.signingkey [key ID]

git commit -S ( <- digitally signs ) -s ( adds "signed off" line )

