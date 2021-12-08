//I'm a plebian so fuck yeah I'm using base types I didn't write, fuck y'all
#include <SynthDefs.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

//Simple ASDR Envelope, might add some fun stuff where you can dynamically add staes later
namespace TTModules {
    class TTEnvGen {

    public:

        TTEnvGen();
        ~TTEnvGen(); 

        enum envtype {
            NONE,
            LINEAR,
            EXP,
            LOG
        };

        void SetAttack(double start,double length,envtype curve);
        void SetSustain(double length);
        void SetDecay(double length,double final,envtype curve);
        void SetRelease(double length,envtype curve);

        void NoAttack();
        void NoSustain();
        void NoDecay();
        void NoRelease();

        //this is what actually builds the envelope
        void GenerateEnvelope();

        //Apply the envelope to your signal
        void ApplyEnvelope(AmpValue * samplesIn,int length);

        int getSize() {
            return envSize;
        }

        void printData();

    private:
        //total length of envelope (in sec)
        double envlength;
        AmpValue* samples;
        envtype statetypes[4];
        //this only matters without streamed things. Which eventaully I want to move to JUCE and not do it this way.
        int envSize;

        //time is in sec, volume in ratio 0-1
        //how long before attack
        double attkStart = 0;
        //how long to max volume
        double attkTime = 0;
        //how long at max volume
        double susTime = 0;
        //Final Amp of decay (ratio of inital 0->1)
        double decayFinal;
        //Final Amp of decay (ratio of inital 0->1)
        double decayTime = 0;
        //How long till silent
        double releaseTime = 0;

        const double initalVolume = 0;
        const double finalVolume = 0;

        enum state {
            ATT,
            SUS,
            DEC,
            REL
        };

        state envstate;

        void BuildAttack(int &index);

        void BuildSustain(int &index);
        
        void BuildDecay(int &index);
        
        void BuildRelease(int &index);

        // returns rate with linear slope
        void xLinear(double intial,double final,int time,int &index);
        // returns rate with exponential slope
        void xExp(double intial,double final,int time,int &index);
        // returns rate with logarithmic slope
        void xLog(double intial,double final,int time,int &index);
        // state machine
        void EnvSM();
    };
}