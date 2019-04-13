# Diffie-Hellman-key-exchange
C++ UPD program allowing two parties to establish a secure communication channel. For simplicity, let us call the program **Host** and **Client**, which are executed by Alice and Bob, respectively.

Alice and Bob share a common password `PW`, which contains 6 numeric characters, and the parameters `(p,g)` for Diffie-Hellman key exchange. They want to establish a secure communication channel that can provide data confidentiality and inetgriy. They aim to achieve this goal via the following steps: (1) use the shared information to establish a shared session key; (2) use the shared session key to secure the communication.

**Step 1** us done via following key exchange protocol:
1. A -> B: *Epw (g^x mod p)*
1. B -> A: *Epw (g^y mod p)*

Alice and Bob then compute the shared key as *K = H (g^xy mod p)* where `H` denotes a cryptographic hash function, Alice and Bob decide to use [RC4](https://en.wikipedia.org/wiki/RC4) as the encryption function, and [SHA-1](https://en.wikipedia.org/wiki/SHA-1) as the Hash function.

After establishing the session key, **step 2** is achieved as follows:
1. whenever Alice wants to send a message `M` to Bob, Alice first computes `H=Hash(K||M)`, and the computes `C=Ek(M||H)` and sends `C` to Bob. Where || denotes the string concatination.
1. upon receiving a ciphertext C, Bob first runs the decryption algorithm to obtain `M||H=Dk(C)`. After that, Bob computes `H'=Hash(K||M)` and checks if `H=H'`. If the equation holds, then Bob accepts `M`; otherwise, Bob rejects the ciphertext.
1. the same operation are performed when Bob sends a message to Alice.
