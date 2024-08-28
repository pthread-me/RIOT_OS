# Flow 2 - description

- This flow is similar to Flow1, except we now have that a single coap rule is shared by all packets, we also use piggybacked acks (which doesn\'t have an effect on compression but just to show that it works when acks are piggybacked)

- The Flow:
    - client sends a post request.
    - server responds with a Success.Content (2.04) packet code, and a packet type set to ACK. With the same token value.

- Rule Assumptions:
    1. The IP address are communicated before hand and as such not sent.
    2. The client operates on UDP port 60000 and the server uses the CoAP assigened port 5684 and as such are both not sent in the SCHC packet.
    3. All tokens used are 1 octet and are in the range \[0, 15] . 
    4. No further assumptions about the token value are made, as such a alll 16 values are mapped and an index value is sent in the SCHC packet.
    5. No assumptions about the type of communication (connection/connetionless).
