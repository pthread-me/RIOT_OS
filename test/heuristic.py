from typing import List, Iterator

MAX_LIST_SIZE = 15

class QField:
    def __init__(self, prio: int, ID: int, value: any):
        self.prio = prio
        self.id = [ID]
        self.value = value
        
    def __str__(self):
        return f"priotity:{self.prio}\tid:{self.id}\tvalue:{self.value}\n"

    def add_id(self, ID):
        n = len(self.id)
        if n > MAX_LIST_SIZE:
            self.id = self.id[1:n]
        self.id.append(ID)

class PrioQueue:
    def __init__(self):
        self._list: List[QField] = []

    def __len__(self)->int:
        return len(self._list)

    def __getitem__(self, index):
        return self._list[index]
    
    def __setitem__(self, index, value):
        self._list[index] = value

    def __iter__(self)->Iterator[QField]:
        return iter(self._list)

    def __str__(self)-> str:
        s = ""
        for f in self:
            s = s.__add__(str(f))

        return s

    def getid(self, ID):
        for f in self:
            if ID in f.id:
                return f.value
        return None

    def degrade(self):
        if len(self._list) == 0:
            return

        for e in self:
            e.prio = e.prio -1
        
            if e.prio == 0:
                self._list.remove(e)

        self._list.sort(key=lambda x: x.prio, reverse=True)


    def add(self, new_field: QField):
        if len(self) is MAX_LIST_SIZE:
            self.degrade()
        
            if len(self) is MAX_LIST_SIZE:
                return
        
        exists: bool = False
        for field in self:
            if field.value == new_field.value:
                field.prio = field.prio+new_field.prio
                exists = True
                field.add_id(new_field.id[0])
                break

        if not exists:
            self._list.append(new_field)
        
        self._list.sort(key=lambda x: x.prio, reverse=True)


#
#if __name__ == "__main__":
#    f1, f2, f3 = QField(1,1,None), QField(2,2,None), QField(2,3,None)

#    q = Queue()
#    q.add(f1)
#    q.add(f2)
#    q.add(f3)

#    for e in q:
#        print(e)
