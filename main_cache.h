#ifndef Cache_H
#define Cache_H
#include "GHB.h"
#include "function.h"
#include "control_unit.h"

class GHB;
class IT;

using namespace std;

class Cache
{
public:
    Cache* LowerLevel;
    Cache* VCPtr = NULL; 
    Cache* Connection = NULL;
    GHB* GHBPtr = nullptr;  
    IT* IndexTable = nullptr; 
    Cache* PrefetchBuffer = nullptr; 
    ControlUnit* ControlPtr = nullptr;  
    string Name;
    bool* SwapHitCheck = new bool(false);
    bool* PrefetchDupe = new bool(false);
    int SwapHitCounter = 0;
    int Size; // In bytes
    int Type; // This is ow many ways are in each set Type 1 = Direct Mapping everything else is set associative and associative mapping
    int BlockSize; //Number of Bytes per block
    int Blocks; //Number of blocks
    int Sets; //Number of sets
    int LSBForSetIndex; //amount of least significant bits needed for the Block Index after the LSBForByte bits
    int TagBits; //amount of bits allocated for the tag after block is written in the cache
    int ExtraBits; 
    int LRUCounterBits;
    int SuperBlocksBits;
    int BlockOffsetBits;
    int Hits = 0;
    int Misses = 0;
    int WriteMiss = 0;
    int ReadMiss = 0;
    int Reads = 0;
    int Writes = 0;
    int WriteBacks = 0;
    int SwapRequests = 0;
    int PrefetchSaves = 0;
    string* WriteBackAddress; //to properly do a write back, the old address of the eviction block is needed not the current therefore we use this pointer to reconstrcut it
    int* TrueWay = new int; //after the tag address is matched with a way, the true way holds the value so we can access that specific way
    string* TagPoint = new string; //pointer for storing only the current tag of the current read/write address
    int* SetOffsetPoint = new int; //pointer for storing the current Set Offset of the address from the read/write address
    vector<vector<string>> VirtualMemory; 
    vector<vector<string>> TagMemory; //tag memory to confirm block identity
    Cache (Cache* connection, string name, int size, int type, int blocksize, Cache * lowerlevel) // cache class constructor
    {   
        Name = name;
        if (Name[0] = 'V')
            Connection = connection;
        LowerLevel = lowerlevel;
        Size = size; 
        Type = type;
        BlockSize = blocksize;
        Blocks = size/blocksize;
        Sets = Blocks/Type;
        ExtraBits = ceil((log2(Type)+2)); //ExtraBits is the bits needed for LRUCounter, DirtyBit, and ValidBit
        LRUCounterBits = ceil(log2(Type)); 
        LSBForSetIndex = ceil(log2(Sets));
        BlockOffsetBits = ceil(log2(BlockSize));
        TagBits = (32 - LSBForSetIndex - BlockOffsetBits); 
        SuperBlocksBits = (28 - BlockOffsetBits);
        Initialize();
    };
    virtual void Initialize () //creates the array that models the cache along with the tag array that confirms its location
    { 
        int m = Sets, n = Type, o = (BlockSize * 8); // o is if I had actual data inside the memory vector
        VirtualMemory.resize(m); //first layer of virtual memory is set
        for (int i = 0; i < m; i++)
        {
            VirtualMemory[i].resize(n); //second layer is the way
            for (int j = 0; j < n; j++)
            {
                VirtualMemory[i][j] = "00";
                string temp = DecToBinary(j); 
                while (temp.length() != LRUCounterBits)
                {
                    temp = '0' + temp;
                }
                VirtualMemory[i][j] = VirtualMemory[i][j] + temp;
                VirtualMemory[i][j] += "0"; //this is the tag bit for the tagged seqeuntial prefetcher
            };
        };
        TagMemory.resize(m); //first layer of tag memory is set
        for (int i = 0; i < m; i++)
        {
            TagMemory[i].resize(n); //second layer is the amount of ways
            for (int j = 0; j < n; j++)
            {
                TagMemory[i][j].resize(TagBits, '0'); //this makes a string as a placeholder for the tag
            };
        };
    };
    void Assign (Cache* ptr, GHB* ghb, IT* indextable, Cache* prefetchbuffer, ControlUnit* controlptr)  
    {
        VCPtr = ptr;
        GHBPtr = ghb;
        IndexTable = indextable;
        PrefetchBuffer = prefetchbuffer;  
        ControlPtr = controlptr;
    }
    virtual bool Read (string bincurrentaddress)
    {   
        while (bincurrentaddress.length() != 32)
        {
            bincurrentaddress.insert(0, 1,'0');
        }
        AddressDecoder(bincurrentaddress);
        bool ValidTag = TagChecker(); //false if no tag matches the address tag, true otherwise
        bool ValidBit = BlockValidityChecker(*TrueWay);
        bool ValidDirty = DirtyBitChecker(*TrueWay);
        if (ValidBit == true && ValidTag == true)
        {
                Hits++; //= data; since I'm only doing blocks with no data IM not implementing the data 
                Reads++;
                LRUCounter();
        }
        else
        {   //this is the read miss 
            *PrefetchDupe = false; 
            int LRUWay = LRUChecker(); 
            Misses++;
            ReadMiss++;
            Reads++;
            ValidDirty = DirtyBitChecker(LRUWay);
            *TrueWay = LRUWay;
            if (PrefetchBuffer != nullptr)
            {
                if (PrefetchBuffer->Read(bincurrentaddress))
                {
                    *PrefetchDupe = true; 
                }
            };
            Allocate(LRUWay, ValidDirty, bincurrentaddress, *PrefetchDupe); 
            LRUCounter();
            bool check = false;
            if (VCPtr != NULL)
            {
                check = *VCPtr->SwapHitCheck;
            }
            if (VCPtr == NULL)
            {
                VirtualMemory[*SetOffsetPoint][LRUWay][0] = '0';
            }
            else if (LowerLevel != NULL && check == false)
            {
                VirtualMemory[*SetOffsetPoint][LRUWay][0] = '0';
            }
            VirtualMemory[*SetOffsetPoint][LRUWay][1] = '1';
            if (VCPtr != NULL) //I used this because the L2 cache doesnt have a VCPtr
                VirtualMemory[*SetOffsetPoint][LRUWay].replace((VirtualMemory[*SetOffsetPoint][LRUWay].length() - 1), 1, "1"); //sets the tag bit to 1 for the prefetch
        }
        if (ControlPtr != nullptr)
        {
            ControlPtr->Update(bincurrentaddress, SuperBlocksBits); //this is called after every address to update the prefetcher
        }
        return false; 
    };//for data I would add a string parameter and set the VirtualMemory equal to that at the correct block
    virtual int Write (string bincurrentaddress) 
    {   
        while (bincurrentaddress.length() != 32)
        {
            bincurrentaddress.insert(0, 1,'0');
        }
        AddressDecoder(bincurrentaddress); //Obtains the decimal Line/Set value of the block as an integer and the Tag Value in binary as a string since the string is the main comparison for a block
        bool ValidTag = TagChecker(); //false if no tag matches the address, tag is true otherwise
        bool ValidBit = BlockValidityChecker(*TrueWay); 
        bool ValidDirty = DirtyBitChecker(*TrueWay);
        if (ValidBit == true && ValidTag == true)
        {
            Hits++; //= data; since I'm only doing blocks with no data IM not implementing the data rn
            Writes++;
            VirtualMemory[*SetOffsetPoint][*TrueWay][0] = '1'; //sets dirty bit to 
            LRUCounter();
            if (ControlPtr != nullptr)
                ControlPtr->Update(bincurrentaddress, SuperBlocksBits);
            return 0;
        }
        else
        {
            *PrefetchDupe = false;
            int LRUWay = LRUChecker();
            Misses++;
            WriteMiss++;
            Writes++;
            ValidDirty = DirtyBitChecker(LRUWay);
            *TrueWay = LRUWay;
            if (PrefetchBuffer != nullptr)
            {
                if (PrefetchBuffer->Read(bincurrentaddress))
                {
                    *PrefetchDupe = true;
                }
            };
            Allocate(LRUWay, ValidDirty, bincurrentaddress, *PrefetchDupe);
            LRUCounter();
            VirtualMemory[*SetOffsetPoint][LRUWay][1] = '1'; //sets valid bit to true
            VirtualMemory[*SetOffsetPoint][LRUWay][0] = '1'; //sets dirty bit to true
            if (VCPtr != NULL && VirtualMemory[*SetOffsetPoint][LRUWay][(VirtualMemory[*SetOffsetPoint][LRUWay].length() - 1)] == '0') //I used this because the L2 cache doesnt have a VCPtr
            {
                VirtualMemory[*SetOffsetPoint][LRUWay].replace((VirtualMemory[*SetOffsetPoint][LRUWay].length() - 1), 1, "1"); //sets the tag bit to 1 for the prefetch
            }
            if (ControlPtr != nullptr)
                ControlPtr->Update(bincurrentaddress, SuperBlocksBits);
            return 1; 
        }
    }; 
    bool BlockValidityChecker (int Way) //valid bit needs to be checked which indicates if next action is a miss or hit
    {
        if (VirtualMemory[*SetOffsetPoint][Way][1] == '1') //if valid bit = 1 then true, otherwise false
        {
            return true;
        } 
        else
        {   
            return false;
        }
    };
    void AddressDecoder(string bincurrentAddress) //basically says what set and way the current block should be looked for
    {   
        int SetOffset = 0;
        string BinTag;
        string BinSet;
        *TagPoint = bincurrentAddress.substr(0, (TagBits));
        bincurrentAddress.erase(0, (TagBits));
        BinSet = bincurrentAddress.substr(0, (LSBForSetIndex)); //possible issue if LSBFORSETINDEx is = 0
        bincurrentAddress.erase(0, (LSBForSetIndex));
        if (LSBForSetIndex != 0)
        {
        SetOffset = stoi(BinSet);

        *SetOffsetPoint = BinaryToDecimal(SetOffset);
        }
        else 
        *SetOffsetPoint = 0;
    }
    bool TagChecker ()
    {
        *TrueWay = 0;
        for (int i = 0; i < TagMemory[*SetOffsetPoint].size(); i++) //checks tag
        {   
            if (*TagPoint == TagMemory[*SetOffsetPoint][i])
            {
                *TrueWay = i;
                return true;
            }
        };
        return false;
    }
    bool DirtyBitChecker(int way)
    {
                if (VirtualMemory[*SetOffsetPoint][way][0] == '1') //if dirty bit = 1 then true, otherwise false (almost the same as valid checker)
        {
            return true;
        } 
        else
        {
            return false;
        }
    }
    bool VCCase(string bincurrentaddress)
    {
        bool VCAccess = true;
        int VCHit = false;
        bool ValidDirty = false;
        for (int i = 0; i < Type; i++)
        {
            bool SetValidCheck = BlockValidityChecker(i);
            if (SetValidCheck == false)
            {   
                VCAccess = false;
                break;
            }
        };
        if (VCAccess == true)
        {   
            VCHit = VCPtr->Read(bincurrentaddress); //will call a swap if there's a hit in the victim cache
            if (VCHit == true)
            {
                SwapHitCounter++;
                return true;
            }
        }
        return false;
    };
    virtual void Allocate(int Temp, bool Dbit, string bincurrentaddress, bool &prefetchdupe) //having the binary address as a parameter even after its decoded is so it can get redecoded
    //in the next write request to the next cache 
    {   
        bool SwapHit = false;
        if (VCPtr != NULL) 
            SwapHit = VCCase(bincurrentaddress); 
        if (Dbit == true && VCPtr == NULL) //this is the dirty bit     
        {
            WriteBack(Temp, bincurrentaddress);
            WriteBacks++;
        }
        if (LowerLevel != NULL && SwapHit == false  && prefetchdupe == false)
            LowerLevel->Read(bincurrentaddress);
        TagMemory[*SetOffsetPoint][Temp] = *TagPoint; 
    };
    virtual void WriteBack (int LRUWay, string bincurrentaddress)
    {       
            string temp = DecToBinary(*SetOffsetPoint);
            while (temp.length() != LSBForSetIndex)
            {
                temp.insert(0, 1, '0');
            };
            string WriteBackAddress = TagMemory[*SetOffsetPoint][LRUWay] + temp; 
            while (WriteBackAddress.length() != 32)
            {
                WriteBackAddress += '0';
            };
            if (LowerLevel != NULL)
                LowerLevel->Write(WriteBackAddress); 
    };  
    int LRUChecker () //might rename. This checks which way was least used for the write/read allocation
    {   
        int max = 0; //I dont think the counter will need more digit than this 
        int Way = 0;
            if (Type != 1)
            {
                for (int i = 0; i < VirtualMemory[*SetOffsetPoint].size(); i++)
                {   
                    int temp = BinaryToDecimal(stoi(VirtualMemory[*SetOffsetPoint][i].substr(2, LRUCounterBits)));
                    if (temp > max)
                    {
                        max = temp;
                        Way = i;
                    }
                }
            }
        return Way;
    };
    void LRUCounter () 
    {   
        if (Type != 1)
        {   
            int Bits = LRUCounterBits;
            string MostRecent = VirtualMemory[*SetOffsetPoint][*TrueWay];
            string Zeros;
            for (int i = 0; i < Bits; i++)
            {
                Zeros += "0";
            };
            VirtualMemory[*SetOffsetPoint][*TrueWay].replace(2, Bits, Zeros);
            for (int i = 0; i < Type; i++)
            {   
                int temp = stoi(VirtualMemory[*SetOffsetPoint][i].substr(2, Bits));
                if(temp < stoi(MostRecent.substr(2, Bits)) && i != *TrueWay)
                {
                    temp = BinaryToDecimal(temp);
                    temp++;
                    string str = DecToBinary(temp);
                    while (str.length() != Bits)
                    {
                        str = '0' + str;
                    }
                    VirtualMemory[*SetOffsetPoint][i].replace(2, Bits, str);
                }  
            }
        }
    };
    virtual void Swap()
    {   
        SwapRequests++;
        int ConnectionTagBits = Connection->TagBits;
        int* ConnectionSet = Connection->SetOffsetPoint; 
        int* ConnectionWay = Connection->TrueWay;
        string TagTempC; //Victim Cache 
        string MemTempC; //Victim Cache
        string MemTempP; //main cache
        string TagTempP; //maincache
        string TagAdder = DecToBinary(*ConnectionSet); //used to add extra bits to the tag since main cache will have less tag bits than victim
        while (TagAdder.length() != Connection->LSBForSetIndex)
            TagAdder = '0' + TagAdder;
        TagTempC = Connection->TagMemory[*ConnectionSet][*ConnectionWay];
        TagTempC += TagAdder;
        MemTempC = Connection->VirtualMemory[*ConnectionSet][*ConnectionWay].substr(0, 2); 
        MemTempP = VirtualMemory[*SetOffsetPoint][*TrueWay].substr(0, 2);
        TagTempP = TagMemory[*SetOffsetPoint][*TrueWay];
        TagTempP.erase(ConnectionTagBits, TagTempP.length()); //delets extra charachters from tag because victim cache is fully associative and thus has a longer tag
        VirtualMemory[0][*TrueWay].replace(0, 2, MemTempC);
        VirtualMemory[0][*TrueWay].replace(VirtualMemory[0][*TrueWay].length() - 1, 1, "0"); //TODO Check if works 
        TagMemory[0][*TrueWay] = TagTempC;
        Connection->VirtualMemory[*ConnectionSet][*ConnectionWay].replace(0, 2 , MemTempP);
        Connection->TagMemory[*ConnectionSet][*ConnectionWay] = TagTempP;
    };
    virtual void Reset(vector<string> pushback) //this isnt pure virtual because the class becomes abstract and cant create any objects with it
    {
    };
    virtual void PrefetchAllocate(vector<string> pushback) {};
};
class Victim : public Cache
{
public: 
    bool* PrefetchDupe = Connection->PrefetchDupe;
    Victim(Cache * connection, int blocks, string name, int size, int type, int blocksize, Cache * lowerlevel):Cache(connection, name, size, type, blocksize, lowerlevel)
    {
        Blocks = blocks;
        Type = Blocks;
        BlockSize = Connection->BlockSize;
    };
    bool Read (string bincurrentaddress)
    {   
        while (bincurrentaddress.length() != 32)
        {
            bincurrentaddress.insert(0, 1,'0');
        }
        AddressDecoder(bincurrentaddress);
        bool ValidTag = TagChecker(); //false if no tag matches the address tag, true otherwise
        bool ValidBit = BlockValidityChecker(*TrueWay);
        bool ValidDirty = DirtyBitChecker(*TrueWay);
        PrefetchDupe = Connection->PrefetchDupe;
        if (ValidBit == true && ValidTag == true)
        {       
            if (*PrefetchDupe == false)
            {   
                Hits++; //= data; since I'm only doing blocks with no data IM not implementing the data rn
                Reads++;
                Swap();       
                *SwapHitCheck = true;
                LRUCounter();
                return true;
            }
            else
            {
                int LRUWay = LRUChecker(); 
                Misses++;
                ReadMiss++;
                Reads++;
                ValidDirty = DirtyBitChecker(LRUWay);
                *TrueWay = LRUWay;
                Allocate(LRUWay, ValidDirty, bincurrentaddress);
                LRUCounter();
                *SwapHitCheck = false;
                return false;
            }
        }
        else
        {   //this is the read miss
            int LRUWay = LRUChecker(); 
            Misses++;
            ReadMiss++;
            Reads++;
            ValidDirty = DirtyBitChecker(LRUWay);
            *TrueWay = LRUWay;
            Allocate(LRUWay, ValidDirty, bincurrentaddress);
            LRUCounter();
            *SwapHitCheck = false;
            return false;
        }
    };
    void Allocate(int Temp, bool Dbit, string bincurrentaddress)
    {   
        if (Dbit == true)
            WriteBack(Temp, bincurrentaddress); 
        int ConnectionTagBits = Connection->TagBits;
        int* ConnectionSet = Connection->SetOffsetPoint;
        int* ConnectionWay = Connection->TrueWay;
        string TagTempC; //Victim Cache 
        string MemTempC; //Victim Cache
        string TagAdder = DecToBinary(*ConnectionSet);
        while (TagAdder.length() != Connection->LSBForSetIndex)
            TagAdder = '0' + TagAdder;
        TagTempC = Connection->TagMemory[*ConnectionSet][*ConnectionWay];
        TagTempC += TagAdder;
        MemTempC = Connection->VirtualMemory[*ConnectionSet][*ConnectionWay].substr(0,2); 
        TagMemory[0][Temp] = TagTempC;
        VirtualMemory[0][Temp].replace(0, 2, MemTempC);
        return;
    }
    void WriteBack (int LRUWay, string bincurrentaddress)
    {       
            string WriteBackAddress = TagMemory[*SetOffsetPoint][LRUWay];

            while (WriteBackAddress.length() != 32)
            {
                WriteBackAddress += '0';
            };
            WriteBacks++;
            Connection->LowerLevel->Write(WriteBackAddress);
    };  
    void Reset (vector<string> pushback) override {}; 
};



class PrefetchBuffer : public Cache
{
public: 
    vector<string> BAMemory; //Buffer address memory
    vector<int> MVec;
    vector<int> SVec;


    bool PrefetchMethod = false; //flase is tagges sequential while true is markov predictor 
    int AddressSize;
    int MarkovTotal = 25; //the total prefetches for this specific algorithm
    int SequentialTotal = 25; //these start at 1 to avoid dividing by 0
    int MarkovHit = 0; //used for threshhold manipulation
    int SequentialHit = 0; //used for threshhold manipulation
    int ST = 0; //actual hit count for tagged sequential
    int MT = 0; // hit count for markov
    //ControlUnit* CU;

    PrefetchBuffer(Cache* connection, string name, int size, int type, int blocksize, Cache * lowerlevel):Cache(connection, name, size, type, blocksize, lowerlevel)
    {
        MVec.resize(0);
        SVec.resize(0);
        Initialize();
    };
    void Initialize ()
    {
        int m = 16; //this is dumb but I dont feel like adding it to the main constructor so edit buffer size here this is in total blocks
        AddressSize = (32 - BlockOffsetBits);
        BAMemory.resize(0); //first layer of virtual memory is set
    }
    bool Read (string bincurrentaddress) //sees if the current address matches the blocks in the buffer 
    {   
        char PrefetchDecider; 
        bool PrefetchHit = false;
        vector<string> temp;
        //clears and replaces the data in the buffer for the most recent miss address markov predictor
        for (int i = 0; i < BAMemory.size(); i++)
        {
            if (BAMemory[i].substr(0, AddressSize) == bincurrentaddress.substr(0, AddressSize))
            {
                if (BAMemory[i][AddressSize] == '1')  //updates the counters for the variable threshhold
                {   
                    MarkovHit++;
                    MT++;
                    MVec.push_back(1);
                    while (MVec.size() > 25)
                    {
                        if (MVec[0] == 1)
                            MarkovHit--;
                        MVec.erase(MVec.begin());
                    }
                }
                else
                {
                    SequentialHit++; 
                    ST++;
                    SVec.push_back(1);

                    while (SVec.size() > 25)
                    {
                        if (SVec[0] == 1)
                            SequentialHit--;
                        SVec.erase(SVec.begin());
                    }
                }
                vector<string>::iterator EraseIt = BAMemory.begin();
                advance(EraseIt, i);
                BAMemory.erase(EraseIt);
                PrefetchSaves++;
                PrefetchHit = true;
                break;
            }
        }
        if (PrefetchHit == false) //this is so the ghb doesnt get flooded with useless entries that were already prefetched
        {
            GHBEntry* NewEntry = GHBPtr->Insert(bincurrentaddress, BlockOffsetBits);
            IndexTable->LookUp(NewEntry);
                if (ControlPtr->PrefetchSet)
                {
                    temp = GHBPtr->MarkovPredictor(NewEntry);
                    PrefetchDecider = '1';
                }
        }
        if (!ControlPtr->PrefetchSet)
        {
            temp = ControlPtr->TaggedSequentialPrefetch(bincurrentaddress, Connection->BlockOffsetBits);
            PrefetchDecider = '0';
        }
        for (int i = 0; i < temp.size(); i++)
        {
            temp[i].push_back(PrefetchDecider);
        }
        PrefetchAllocate(temp, bincurrentaddress);
        ControlPtr->UpdateThreshHold(MarkovTotal, MarkovHit, SequentialTotal, SequentialHit);
        return PrefetchHit;
    };
    void PrefetchAllocate (vector<string> pushback, string bincurrent) //sets the BAMemory and checks for duplicates
    {   
        int Index1 = 0; 
        for (string &element1 : pushback)
        {
            for (string &element2 : BAMemory)  //this checks for duplicates in prefetch buffer and pushback not L1
            {
                if (element1 == element2)
                {
                    pushback[Index1] = "0";
                }
            }
        Index1++;
        }
        
        pushback.erase(remove(pushback.begin(), pushback.end(), "0"), pushback.end());

        if (pushback.size() == 0)
        {
            pushback = ControlPtr->TaggedSequentialPrefetch(bincurrent, Connection->BlockOffsetBits);
            char PrefetchDec = '0';
            for (int i = 0; i < pushback.size(); i++)
            {
                pushback[i].push_back(PrefetchDec); 
            }
        }
        BAMemory.insert(BAMemory.end(), pushback.begin(), pushback.end());
        while (BAMemory.size() > 16)
        {
            string temp = BAMemory[0].substr(BAMemory[0].length() - 1, 1);
            if (temp == "1")
            {
                MVec.push_back(0);
                //SVec.push_back(1);
                //SequentialHit++;
                while (MVec.size() > 25)
                {
                    if (MVec[0] == 1)
                        MarkovHit--;
                    MVec.erase(MVec.begin());
                }
            }
            else
            {
                SVec.push_back(0);
                while (SVec.size() > 25)
                {
                    if (SVec[0] == 1)
                        SequentialHit--;
                    SVec.erase(SVec.begin());
                }
            }

            BAMemory.erase(BAMemory.begin());
        }
    };
};

#endif