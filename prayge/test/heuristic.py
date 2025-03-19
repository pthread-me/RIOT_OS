from typing import List, Tuple

MAX_LIST_SIZE = 64

rules: List[any]

uri_queue:  List[Tuple[int, int, List[any]]]
app_pre:    List[Tuple[int, int, any]]
app_id:     List[Tuple[int, int, any]]
dev_pre:    List[Tuple[int, int, any]]
dev_id:     List[Tuple[int, int, any]]



def degrade_queue(queue: List[Tuple[int, int, any]]):
    for e in queue:
        e[0] = e[0] -1
        if e[0] == 0:
            queue.remove(e)
        
def update_queue(queue: List[Tuple[int, int, any]], field: Tuple[int, int, any]):
    if len(queue) is MAX_LIST_SIZE:
        degrade_queue(queue)
        
        if len(queue) is MAX_LIST_SIZE:
            return
        
        queue.append(field)

