from typing import Counter
import matplotlib.pyplot as plt
from numpy.core.fromnumeric import sort
import coap
import numpy as np
from scapy.all import *
from scipy.stats import beta

SAMPLE_SIZE = 10000 
VARIABLES = 20
ALPHA = 2
BETA = 7

dev_pre = [
    "1a3b:4c5d:6e7f:8f91",
    "2b4c:5d6e:7f8f:911a",
    "3c5d:6e7f:8f91:1a2b",
    "4d6e:7f8f:911a:2b3c",
    "5e7f:8f91:1a2b:3c4d",
    "6f8f:911a:2b3c:4d5e",
    "7f91:1a2b:3c4d:5e6f",
    "811a:2b3c:4d5e:6f7f",
    "912b:3c4d:5e6f:7f81",
    "1a3c:4d5e:6f7f:8191",
    "2b4d:5e6f:7f81:911a",
    "3c5e:6f7f:8191:1a2b",
    "4d6f:7f81:911a:2b3c",
    "5e7f:8191:1a2b:3c4d",
    "6f81:911a:2b3c:4d5e",
    "7f91:1a2b:3c4d:5e6f",
    "811a:2b3c:4d5e:6f7f",
    "912b:3c4d:5e6f:7f81",
    "1a3c:4d5e:6f7f:8191",
    "2b4d:5e6f:7f81:911a"
]
dev_ip = [
    "ffff:1111:2222:3333",
    "1111:2222:3333:4444",
    "2222:3333:4444:5555",
    "3333:4444:5555:6666",
    "4444:5555:6666:7777",
    "5555:6666:7777:8888",
    "6666:7777:8888:9999",
    "7777:8888:9999:aaaa",
    "8888:9999:aaaa:bbbb",
    "9999:aaaa:bbbb:cccc",
    "aaaa:bbbb:cccc:dddd",
    "bbbb:cccc:dddd:eeee",
    "cccc:dddd:eeee:ffff",
    "dddd:eeee:ffff:1111",
    "eeee:ffff:1111:2222",
    "ffff:1111:2222:3333",
    "1111:2222:3333:4444",
    "2222:3333:4444:5555",
    "eeee:ffff:1111:2222",
    "3333:4444:5555:6666"
]


app_pre = [
    "2b4d:5e6f:7f81:911a",
    "3c5e:6f7f:8191:1a2b",
    "4d6f:7f81:911a:2b3c",
    "5e7f:8191:1a2b:3c4d",
    "6f81:911a:2b3c:4d5e",
    "7f91:1a2b:3c4d:5e6f",
    "811a:2b3c:4d5e:6f7f",
    "912b:3c4d:5e6f:7f81",
    "1a3c:4d5e:6f7f:8191",
    "2b4d:5e6f:7f81:911a",
    "3c5e:6f7f:8191:1a2b",
    "4d6f:7f81:911a:2b3c",
    "5e7f:8191:1a2b:3c4d",
    "6f81:911a:2b3c:4d5e",
    "7f91:1a2b:3c4d:5e6f",
    "811a:2b3c:4d5e:6f7f",
    "912b:3c4d:5e6f:7f81",
    "1a3c:4d5e:6f7f:8191",
    "2b4d:5e6f:7f81:911a",
    "3c5e:6f7f:8191:1a2b"
]
app_ip = [
    "aaaa:bbbb:cccc:dddd",
    "bbbb:cccc:dddd:eeee",
    "cccc:dddd:eeee:ffff",
    "dddd:eeee:ffff:1111",
    "eeee:ffff:1111:2222",
    "ffff:1111:2222:3333",
    "1111:2222:3333:4444",
    "2222:3333:4444:5555",
    "3333:4444:5555:6666",
    "4444:5555:6666:7777",
    "5555:6666:7777:8888",
    "6666:7777:8888:9999",
    "7777:8888:9999:aaaa",
    "8888:9999:aaaa:bbbb",
    "9999:aaaa:bbbb:cccc",
    "aaaa:bbbb:cccc:dddd",
    "bbbb:cccc:dddd:eeee",
    "cccc:dddd:eeee:ffff",
    "dddd:eeee:ffff:1111",
    "eeee:ffff:1111:2222",
]

uri = [
    "login",
    "logout",
    "register" ,
    "profile" ,
    "dashboard",
    "setting",
    "account",
    "notification",
    "message",
    "search",
    "uploa",
    "download",
    "file",
    "image",
    "post",
    "temerature",
    "time",
    "location",
    "setup",
    "stop"
]


def sample(sample):
    global n, spread
    weights = beta.rvs(a=ALPHA, b=BETA, size=VARIABLES)
    weights /= weights.sum()    # normalizing the weights
#    plt.bar(range(0,100), weights)
#    plt.show()
    return np.random.choice(sample, size=SAMPLE_SIZE, replace=True, p =weights)
    

        
def main():
    global uri, dev_pre, dev_ip, app_pre, app_ip

    output = open("sample_1.txt", 'w')
    uri_X = sample(uri)
    dev_pre_X = sample(dev_pre)
    dev_ip_X = sample(dev_ip)
    app_pre_X = sample(app_pre)
    app_ip_X = sample(app_ip)

    output.write(' '.join(uri_X)+'\n')
    output.write(' '.join(dev_pre_X)+'\n')
    output.write(' '.join(dev_ip_X)+'\n')
    output.write(' '.join(app_pre_X)+'\n')
    output.write(' '.join(app_ip_X)+'\n')
    
if __name__ == "__main__":
    main()
