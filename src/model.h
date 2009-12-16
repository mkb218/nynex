/*
 *  model.h
 *  fs1rgen
 *
 *  Created by Matthew J Kane on 12/15/09.
 *  Copyright 2009 Akamai Technologies. All rights reserved.
 *
 */
#ifndef H2P_FS1RGEN_MODEL
#define H2P_FS1RGEN_MODEL

#include <exception>
#include "ga/ga.h"

namespace fs1rgen {
    class Model {
    public:
        void initialize(GAGenome&) const;
        int mutate(GAGenome&,float) const;
        float evaluate(GAGenome&) const;
        float compare(const GAGenome&, const GAGenome&) const;
        int spawn(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*) const;
        void copy(const Model * model);
    private:
        void *data;
    };

    /* A Genome object fulfills a given model. The static methods given to 
     GALib will use the model to perform the genetic operators. */
    class Genome: public GAGenome {
    public:
        GADefineIdentity("fs1rgen_genome", 218);
        static void Initializer(GAGenome & g) { dynamic_cast<Genome&>(g).model_->initialize(g); }
        static float Evaluator(GAGenome & g) { return dynamic_cast<Genome&>(g).model_->evaluate(g); }
        static int Mutator(GAGenome & g, float p)  { return dynamic_cast<Genome&>(g).model_->mutate(g, p); }
        static float Comparator(const GAGenome & g, const GAGenome & h)  { dynamic_cast<const Genome&>(g).model_->compare(g,h); }
        static int Crossover(const GAGenome & mom, const GAGenome & dad, GAGenome *bro, GAGenome *sis)  { return dynamic_cast<const Genome&>(mom).model_->spawn(mom,dad,bro,sis); }
        virtual void copy(const GAGenome & g) { model_->copy( dynamic_cast<const Genome&>(g).model_ ); };
        virtual GAGenome * clone(GAGenome::CloneMethod) { return new Genome(*this); };
        Genome() : GAGenome(Initializer, Mutator, Comparator) { crossover(Crossover); evaluator(Evaluator); }
        Genome(GAGenome & other) { copy(other); }
        Genome & operator=(const GAGenome& orig) {
            if(&orig != this) copy(orig);
            return *this;
        }
    private:
        // what should model be?
        Model *model_;
    };
    
    
    class Exception : public std::exception {
    };
    
    class Evaluator { // interace for evaluator
    public:
        static Evaluator * getInstance();
        static void setInstance(Evaluator *);
        virtual float evaluate(GAGenome &) = 0;
        void sendToSynth() throw (Exception);
    private:
        Evaluator() {}; // prevent instantiation
//        static lock_t singletonLock_;
        static Evaluator * instance_;
    };
        
}  

#endif