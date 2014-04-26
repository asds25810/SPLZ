#include <memory.h>
class PriorityQueue
{
	private:
        int _count;
		int _length;
        int *_heap;
        static const int DefaultCapacity = 100;
        int *weight;
        int *hash;

	public:
		PriorityQueue(int capacity,int *weight,int *hash);
		~PriorityQueue();
		int HeapLeftChild(int i);
		int HeapParent(int i);
		int HeapRightFromLeft(int i);
		void Pop();
		void Push(int value);
		void Adjust(int index);
		int Count();
		int Top();
};

// Methods
PriorityQueue::PriorityQueue(int capacity,int *weight,int *hash)
{
	this->_length=(capacity > 0) ? capacity : DefaultCapacity;
    this->_heap = new int[this->_length];
    this->_count = 0;
    this->weight =weight ;
    this->hash = hash;
}

int PriorityQueue::HeapLeftChild(int i)
{
    return ((i * 2) + 1);
}

int PriorityQueue::HeapParent(int i)
{
    return ((i - 1) / 2);
}

int PriorityQueue::HeapRightFromLeft(int i)
{
    return (i + 1);
}

void PriorityQueue::Pop()
{
    int z;
    //hash[this->Top] = -1;
    if (this->_count > 0)
    {
        int i = 0;
        for (int j = HeapLeftChild(i); j < this->_count; j = HeapLeftChild(i))
        {
            int index = HeapRightFromLeft(j);
            int num4 = ((index < this->_count) && (weight[this->_heap[index]]<weight[ this->_heap[j]])) ? index : j;
            if (weight[this->_heap[this->_count - 1]] <= weight[this->_heap[num4]])
                break;
            this->_heap[i] = this->_heap[num4];
            hash[this->_heap[i]] = i;
            i = num4;
        }
        this->_heap[i] = this->_heap[this->_count - 1];
        hash[this->_heap[i]] = i;
    }
    this->_count--;
}

void PriorityQueue::Push(int value)
{
    int index,i;
  
    if (this->_count == this->_length )
    {
        int *localArray = new int[this->_count * 2];
        /*for (int j = 0; j < this->_count; j++)
        {
            localArray[j] = this->_heap[j];
        }*/
		memcpy(localArray,this->_heap,sizeof(int)*this->_count);
		delete this->_heap;
        this->_heap = localArray; 
		this->_length *=2;
    }

    i = this->_count;
    while (i > 0)
    {
        index = HeapParent(i);
        if (weight[value]>=weight[this->_heap[index]])
        {
            break;
        }
        this->_heap[i] = this->_heap[index];
        hash[this->_heap[i]] = i;
        i = index;
    }
    this->_heap[i] = value;
    hash[this->_heap[i]] = i;
    this->_count++;
}

void PriorityQueue::Adjust(int index)
{
    int i = index;
    int value = this->_heap[index];
    while (i > 0)
    {
        int p = HeapParent(i);
        if (weight[value] >= weight[this->_heap[p]])
        {
            break;
        }
        this->_heap[i] = this->_heap[p];
        hash[this->_heap[i]] = i;
        i = p;
    }
    this->_heap[i] = value;
    hash[this->_heap[i]] = i;
}



// Properties
int PriorityQueue::Count()
{
    return this->_count;            
}

int PriorityQueue::Top()           
{
	if(this->_count>0)
		return this->_heap[0];
	else 
		return -1;
}

PriorityQueue::~PriorityQueue()
{
	delete this->_heap;
}
