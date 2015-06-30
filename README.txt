AES file encryption/decryption tools.  Written by Kevin Neale (c) 2015
======================================================================

A collection of tools to arbitrarily encrypt/decrypt file using OpenSSL AES 256 CBC with a 128 bit block size.

Note! It's for demonstration purposes only and uses fixed key and initialisation vector.  
It should not be used for production purposes!

To build:

1) $ make

A unit test is executed automatically to check the functionality.

The 2 tools produced are:

1) encrypt - A tool to encrypt an input file.
2) decrypt - A tool to decrypt an input file.

Note! Tested on a Ubuntu linux distribution ONLY using:

gcc version 4.8.2 (Ubuntu 4.8.2-19ubuntu1)
OpenSSL 1.0.1f 6 Jan 2014

End Of File
