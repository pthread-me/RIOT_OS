class COAP():
    def __init__(self, fields):
        self.ver        = fields["version"]
        self.type       = fields["type"]
        self.tkn_len    = fields["tkn_len"]
        self.code       = fields["code"]
        self.msg_id     = fields["msg_id"]
        self.token      = fields["token"]
        self.opt        = fields["opt"]

