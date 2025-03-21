#include "function.h"

using namespace std;

struct AddressEntry
{
public:
    string Address = "0";
    unsigned int Counter = 0;
    unsigned int LRUCounter = 0;
    AddressEntry() {}; 
};

class ControlUnit
{
public:
    bool PrefetchSet = true; //false is the tagged prefetcher true is the markov prefetcher
    vector<AddressEntry> AddressTracker; 
    int PushBackSize = 1;
    double ThreshHold = 50;
    ControlUnit (int pushbacksize)
    {
        AddressTracker.resize(16); 
        for (int i = 0; i < AddressTracker.size(); i++)
        {
            AddressTracker[i].LRUCounter = i;
        };
        PushBackSize = pushbacksize;
    };
    bool UpdateThreshHold (int markovt, int markovhit, int sequentialt, int sequentialhit) //returns true if kickback occured else false
    {
        double MarkovPerc = static_cast<double>(markovhit)/markovt;
        double SequentialPerc = static_cast<double>(sequentialhit)/sequentialt;
        double ThreshHoldMultiplier = MarkovPerc/SequentialPerc;
        if (ThreshHoldMultiplier > 1)
        {
            ThreshHoldMultiplier = ThreshHoldMultiplier / 10;
            ThreshHoldMultiplier = 1 + ThreshHoldMultiplier;
        }
        else 
        {
            ThreshHoldMultiplier = ThreshHoldMultiplier / 10;
            ThreshHoldMultiplier = 1 - ThreshHoldMultiplier;
        }
        ThreshHold = ThreshHold * ThreshHoldMultiplier;
        if (ThreshHold > 511)
            ThreshHold = 511;
        if (ThreshHold < 1)
            ThreshHold = 1;
        return false;

    }
    void Update (string bincurrentaddress, int SuperBlockBits)
    {
        bool found = false; 
        string SubBin = bincurrentaddress.substr(0, SuperBlockBits);
        for (int i = 0; i < AddressTracker.size(); i++) 
        {
            //cout << SubBin << endl;
            //cout << AddressTracker[i].Address << " AddressTrack" << endl;
            if (SubBin == AddressTracker[i].Address)
                {
                    //cout << AddressTracker[i].Counter << endl;
                    AddressTracker[i].Counter++;
                    LRUReset(AddressTracker, i); //same as lrucounter just named differently
                    found = true;
                    if (AddressTracker[i].Counter > ThreshHold) //this number determines how easily the tagged sequential prefetcher will be active
                        PrefetchSet = false;
                    else 
                        PrefetchSet = true;
                    break;
                }
        };
        if (!found)
        {
            int max = 0;
            int index = 0;
            for (int i = 0; i < AddressTracker.size(); i++)
            {
                if (AddressTracker[i].LRUCounter > max)
                {
                    max = AddressTracker[i].LRUCounter;
                    index = i;
                }
            };
            AddressTracker[index].Address = bincurrentaddress.substr(0, SuperBlockBits);
            AddressTracker[index].Counter = 0;
            LRUReset(AddressTracker, index);
        }

    };
    void LRUReset (vector<AddressEntry> &refvector, int index) //too lazy so just harded coded this in
    {
        for (int i = 0; i < refvector.size(); i++)
        {
            if (refvector[i].LRUCounter < refvector[index].LRUCounter)
                refvector[i].LRUCounter++;
        }
        refvector[index].LRUCounter = 0;
    };
    vector<string> TaggedSequentialPrefetch (string bincurrentaddress, int blockoffset)
    {
        vector<string> pushback;
        pushback.resize(PushBackSize);
        string temp = HashBinToDec(bincurrentaddress.substr(0, (32 - blockoffset)));
        int intbincurrent = stoi(temp);
        for (int i = 0; i < PushBackSize; i++)
        {
            intbincurrent++;
            string convert = DecToBinary(intbincurrent);
            while (convert.length() != 28)
                convert = '0' + convert;
            pushback[i] = convert;
        };
        return pushback;

    };
}; 