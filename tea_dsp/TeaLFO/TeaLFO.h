/*
 * LFO, will be simple to start but will likely add stuff as my adventures continue
 *
 */

namespace TTModules {

    class TeaLFO {

    public:

        TeaLFO();

        TeaLFO(float aphi);

        ~TeaLFO();

        // wavetypes, only doing two for now    
        enum wavetype {
            SINE,
            SQUARE,
            //TRIANG
        };

        //modulate the input sample
        void modulate(float* sample, bool pos);
        void add(float* sample, bool pos);
        void subtract(float* sample, bool pos);

        void setShape(wavetype shape);
        void setFs(float afs);
        void setFreq(float afreq);
        void setPhi(float aphi);

        int getBufSize();

        //debug functions
        void print(float* wave, int size);

    private:
        // set for initial,phase
        float initphi;
        // sample rate
        float fs;
        // frequency
        float freq;
        // Wave shape
        wavetype shape;
        // Period of wave
        int T_size;


        int samptime;
        float* lfobuf;

        void populateLFO();
    };
};