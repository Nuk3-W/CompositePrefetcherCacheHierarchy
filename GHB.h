#ifndef GHB_H
#define GHB_H

#include "function.h"

class GHB;

struct GHBEntry
{   
    string Address;
    GHBEntry* Next = nullptr;
    GHBEntry* Previous = nullptr;
    GHBEntry* Duplicate = nullptr;
    int HashStep = 0;
    GHBEntry(string addr) : Address(addr), Next(nullptr) {} 
};
struct IT 
{   
    GHB* Connection;
    vector<GHBEntry*> IndexTable; 
    size_t BufferSize;
    int BlockSize;
    IT (size_t buffersize, int blocksize, GHB* connection) : BufferSize(buffersize), BlockSize(blocksize), Connection(connection) 
    {
        IndexTable.resize(BufferSize, nullptr); 
    };
    void LookUp (GHBEntry* entry) 
    {   
        int BlockBits = (ceil(log2(BlockSize)));
        int Index = HashFunction(entry->Address, BufferSize); //checkk if the hash function takes in a string or int
        if (IndexTable[Index] == nullptr)
        {
            IndexTable[Index] = entry;
        }
        else if (IndexTable[Index]->Address == entry->Address)
        {   
            entry->Duplicate = IndexTable[Index];
            IndexTable[Index] = entry;
        }
        else
        {
            while(IndexTable[Index] != nullptr) //here we iterate through the linked list to see if the address has been indexed with collisions
            {
                if(IndexTable[Index]->Address == entry->Address) 
                {
                    entry->Duplicate = IndexTable[Index];
                    IndexTable[Index] = entry;
                    return;
                }
                if (Index != 511)
                {
                    Index++;
                }
                else
                    Index = 0;
                entry->HashStep++;
            };
        }
        IndexTable[Index] = entry; 
    };
    void DeletionOfDuplicate (GHBEntry* entry)
    {
        int Index = HashFunction(entry->Address, BufferSize);
        Index += (entry->HashStep); 
        if (Index > 511)
            Index = Index - 512; //the hashstep was going over the buffersize of the table
        if (IndexTable[Index]->Address == entry->Address)
        {
            GHBEntry* Current = IndexTable[Index];
            if (Current == entry) //this is used when the head of the ghb is also the first instance of itself in the ghb
            {
                IndexTable[Index] = nullptr;
                return;
            }
            while (Current->Duplicate != entry)
            {
                Current = Current->Duplicate;
            };
            Current->Duplicate = nullptr; //dumb way to do it but oh well
        }
        else
        {
            while (IndexTable[Index]->Address != entry->Address)
            {
                Index++;
            };
            GHBEntry* Current = IndexTable[Index];
            while (Current->Duplicate != entry)
            {
                Current = Current->Duplicate;
            };
            Current->Duplicate = nullptr;
        }
    };
};
class GHB
{
public:
    struct MarkovEntry
    {
        int Counter = 0;
        string Address = "0"; 
    };  
    IT* Connection;
    static bool CompareByValue(const MarkovEntry& a, const MarkovEntry& b); 
    GHBEntry* Head = nullptr;
    GHBEntry* Tail = nullptr;
    size_t BufferSize = 0; 
    size_t Count = 0;
    int PushBackSize;
    GHB (size_t buffersize, int pushbacksize) : BufferSize(buffersize), PushBackSize(pushbacksize) {};
    void ConnectToIT (IT* connection) //was gonna be a circular dependency for IT and GHB so added assign fucntion
    {
        Connection = connection;
    }
    GHBEntry* Insert (string missaddress, int blockbits) 
    {    
        missaddress = missaddress.substr(0, 32 - blockbits);
        GHBEntry* NewEntry = new GHBEntry(missaddress); //this is a pointer the heap keep the memory even after pointer is pushed from ghb
        if (Count != BufferSize)
            Count++;
        else 
        {
            GHBEntry* OldHead = Head;
            Head = Head->Next;
            Connection->DeletionOfDuplicate(OldHead); //used to delete pointers that werent updated after deletion
            delete OldHead;
        }
        if (Tail != nullptr)
        {   
            Tail->Next = NewEntry;
            NewEntry->Previous = Tail;
            Tail = NewEntry; 
        }
        else
        {   
            Head = NewEntry;
            Tail = NewEntry;
            NewEntry->Next = NewEntry;
            NewEntry->Previous = NewEntry;
        }
        return NewEntry;
    };
    struct CompareByCounter 
    {
        bool operator()(const MarkovEntry& a, const MarkovEntry& b) const 
        {
            return a.Counter > b.Counter;
        }
    };
    vector<string> MarkovPredictor(GHBEntry* missreference) 
    {   
        vector<MarkovEntry> MarkovTable;
        MarkovTable.resize(BufferSize); 
        GHBEntry* Current = missreference->Duplicate; 
        vector<string> Max; //this is to keep track of the most entries after the miss address
        Max.reserve(16);//can go up to 16 but im using the size to make an iterator so keep the actual vector smaller than 16 unless necessary
        if (Current == nullptr)
        {
            return Max;
        }
        while (Current != nullptr)
        {   
            int temp = HashFunction(Current->Next->Address, 512); //gets the index based on the hash function          
            if (MarkovTable[temp].Address == "0")
            {
                MarkovTable[temp].Counter++;
                MarkovTable[temp].Address = Current->Next->Address;
            }
            else if (MarkovTable[temp].Address == Current->Next->Address)
            {   
                MarkovTable[temp].Counter++;
            }
            else
            {
                while (MarkovTable[temp].Address != Current->Next->Address)
                {   
                    if (MarkovTable[temp].Address == "0") //this checks if we hit a blank entry 
                    {
                        MarkovTable[temp].Address = Current->Next->Address;
                        MarkovTable[temp].Counter++;
                        break;
                    }
                    else if (MarkovTable[temp].Address == Current->Next->Address)
                    {
                        MarkovTable[temp].Counter++;
                        break;
                    }
                    else if (temp == 512)
                    {
                        temp = 0;
                    }
                    else
                    {
                        temp++; //for correction im just using a sequential search after the hash
                    }
                }
            }
            Current = Current->Duplicate;
        }
        sort(MarkovTable.begin(), MarkovTable.end(), CompareByCounter()); 
        for (int i = 0; i < PushBackSize; i++)  //iterate however many times to get the highest probable next addresses based on markov predictor
        { 
            if (MarkovTable[i].Address != "0")
                Max.push_back(MarkovTable[i].Address);
        };
        return Max;
    };
};

#endif