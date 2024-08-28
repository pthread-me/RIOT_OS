# Flow 1 - description

- We assume a simple 4 packet communication between 2 coap nodes with non-piggy backed acknoledgments

- The Flow:
    - client sends a get request with a token length of 8 holding a token value 0x01.
    - server sends an empty acknoledgment.
    - server responds with a Success.Content (2.04) packet. With the same token value.
    - client sends an emty acknoledgment.

- Rule Assumptions:
    1. The IP address are communicated before hand and as such not sent.
    2. The client operates on UDP port 60000 and the server uses the CoAP assigened port 5684 and as such are both not sent in the SCHC packet.
    3. All tokens used are 1 octet and are in the range \[0, 15] . 
    4. No further assumptions about the token value are made, as such a alll 16 values are mapped and an index value is sent in the SCHC packet.
    5. No assumptions about the type of communication (connection/connetionless).
