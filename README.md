# Diffie-Hellman-key-exchange
C++ UPD program allowing two parties to establish a secure communication channel. For simplicity, let us call the program **Host** and **Client**, which are executed by Alice and Bob, respectively.

Alice and Bob share a common password `PW`, which contains 6 numeric characters, and the parameters `(p,g)` for Diffie-Hellman key exchange. They want to establish a secure communication channel that can provide daa confidentiality and inetgriy. They aim to achieve this goal via the following steps:
1. A -> B: *Epw (g^x mod p)*
1. B -> A: *Epw (g^y mod p)*
