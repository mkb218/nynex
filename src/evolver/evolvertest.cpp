/*
 *  evolvertest.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 2/2/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#include <iostream>
#include <cstdlib>
#include "evolvertest.h"
#include "evolver.h"
#include "composition.h"
#include "ratings.h"
#include "ga/ga.h"

using namespace nynex;

int main(int argc, char **argv) {
    SampleBank & bank = SampleBank::getInstance();
    bank.setChannels(2);
    bank.setSampleRate(44100);
    bank.setSampleSize(2);
    bank.setTmpDir("/tmp/nynex");
    bank.setSampleDir("/Users/makane/code/nynex/samples");
    Composition c;
    bank.initComposition(c);
    c.bounceToFile("/Users/makane/code/nynex/output/whatwhat.mp3");
    for (int i = 0; i <= 100; ++i) {
        Ratings::getInstance().addRating(c.getObjectId(), random() % 5);
    }
    std::cout << Ratings::getInstance().avgRatingForId(c.getObjectId()) << std::endl;
    
    std::cout << "setting up galib" << std::endl;
    
    Composition *unserialized = Composition::unserialize("13:JNN034-TheRunnerTheme-Scene9_vbr.wav:353:unfields07_04_-_grincement_amarres.wav:106:01 C'mon Let's Go! (Bitzblitz Mix).wav:1565:UG002 - A1 - Hate Me Not.wav:1479:JNN075-05-Concrete.wav:359:okfunny.wav:335:rsvp003_13_Dooshbag_MoozeBlaster_Remix_vbr.wav:1309:unfields11_12_-_dca_foggy_bottom.wav:215:B2 - lon rmx.wav:8913:rsvp003_08_LTB_Nisei23_Remix_vbr.wav:112:05 I'm a Groover, Not A Raver (Live Benjorg Mix).wav:3108:Okmusc8.wav:635:rsvp_po_09_Horizon_vbr.wav:939:UG002 - B1 - Inverted Burning People.wav:1855:unfields04_01_-_subway_coming_going.wav:216:UG003 - B1 - Dan Bensons Project - Ladies Get On The Flo'.wav:1341:04_ASpotlightOnNoahsArc_monsterx_vbr.wav:152:B2 - lon rmx.wav:1013:04 They're A' Happy (Chai Tea Mix).wav:2569:04_Small_Radio_-_I_Am_Ricky_Richardo_Le_Migre_EP.wav:874:emp063_06_Just_Because_vbr.wav:524:rsvp_po_10_Improv_Jam_2_vbr.wav:764:unfields05_01_-_entering_kb.wav:388:JNN075-03-IAmInControlNow.wav:1832:rsvp_feep_07_Theyre_A_Happy_Adrian_Carter_Remix_vbr.wav:2303:chiptune_megamix.wav:17288:JNN075-09-Nano90210.wav:1609:unfields11_15_-_ibz_tranquilo.wav:775:MP-RAGE.wav:104:the_hexdump_protocol_002.wav:5628:the_hexdump_protocol_001.wav:1416:unfields04_12_-_at_the_market_4.wav:117:the_hexdump_protocol_001.wav:536:chiptune_megamix.wav:1816:JNN075-10-GB_Treats.wav:7912:rsvp003_11_Tooth_And_Claw_Clutter_Remix_vbr.wav:186:Chaircrusher-SergeantRidiculous.wav:1118:01 C'mon Let's Go! (Bitzblitz Mix).wav:1795:B1 - engine.wav:1565:ok5.wav:110:rsvp_feep_01_CMon_Lets_Go_Phasen_Remix_vbr.wav:3717:SergeantRidiculous-SeanDeasonRmx.wav:591:UG003 - B1 - Dan Bensons Project - Ladies Get On The Flo'.wav:373:JNN059-01-PakIt_vbr.wav:857:rsvp_gulu_01_Ashtray_vbr.wav:580:01_Small_Radio_-_Ashtray_Le_Migre_EP.wav:10:chiptune_megamix.wav:3340:rsvp_po_04_A_Spotlight_On_Noahs_Arc_vbr.wav:364:chiptune_megamix.wav:12810:05 I'm a Groover, Not A Raver (Live Benjorg Mix).wav:2225:rsvp_feep_06_A_Dont_Care_Emotiquon_Remix_vbr.wav:2522:unfields02_11_-_steel-deck_mower1.wav:7:chiptune_megamix.wav:5373:DTI.wav:161:JNN075-10-GB_Treats.wav:3157:rsvp_feep_03_Theyre_A_Happy_Vic_Spanner_Remix_vbr.wav:378:UG003 - A2 - Dan Bensons Project - People Tell Me.wav:882:B1 - engine.wav:2722:chiptune_megamix.wav:22725:JNN059-04-Titty_vbr.wav:1018:05_ASpotlightOnNoahsArc_Indexwerk_vbr.wav:130:rsvp_gulu_07_So_vbr.wav:2471:rsvp003_07_LTB_Andrea_Valvini_Remix_vbr.wav:2455:rsvp_feep_07_Theyre_A_Happy_Adrian_Carter_Remix_vbr.wav:120:rsvp_feep_01_CMon_Lets_Go_Phasen_Remix_vbr.wav:2630:UG002 - A2 - Hate Me Not (Fred Giannelli Remix).wav:1432:the_hexdump_protocol_001.wav:6529:JNN059-03-Funny_vbr.wav:790:the_hexdump_protocol_001.wav:1602:the_hexdump_protocol_002.wav:7949:SergeantRidiculous-DavePowersRmx.wav:610:rsvp003_02_Dooshbag_Benzene_Vapours_Remix_vbr.wav:744:04 They're A' Happy (Chai Tea Mix).wav:378:rsvp_feep_01_CMon_Lets_Go_Phasen_Remix_vbr.wav:708:rsvp_gulu_03_Caylas_Third_vbr.wav:1829:rsvp_feep_09_CMon_Brother_Lets_Go_The_A_Frames_Remix_vbr.wav:2830:Okimreal.wav:147:JNN059-04-Titty_vbr.wav:1525:06_Glide_Away_Industry56_vbr.wav:750:07_GlideAway_smallcolin_vbr.wav:2118:06 Pk Trismegistus.wav:662:B2 - lon rmx.wav:4995:02 Pores.wav:5509:A2 - ups and kicks.wav:1908:rsvp003_11_Tooth_And_Claw_Clutter_Remix_vbr.wav:626:emp063_01_LTB_vbr.wav:44:rsvp_po_07_I_Dont_Wanna_Talk_To_You_vbr.wav:589:03_GlideAway_phasen_vbr.wav:1298:UG002 - B1 - Inverted Burning People.wav:1760:rsvp_feep_04_Junks_Fae_Ecosse_Megamix_vbr.wav:693:UG003 - B1 - Dan Bensons Project - Ladies Get On The Flo'.wav:1102:okpoem.wav:283:unfields01_24_-_trainbreaks2.wav:7:rsvp_po_03_Ashtray_vbr.wav:1194:08_GlideAway_prprt_vbr.wav:1991;");
    unserialized->bounceToFile("/Users/makane/code/nynex/output/unser.wav");
    
    Evolver e;
    Ratings & r = Ratings::getInstance();
    e.initGA(0.1, 3, gaFalse);
    while (e.getGA().generation() < 3) {
        const GAPopulation & pop = e.getPop();
        for (size_t i = 0; i < pop.size(); ++i) {
            Composition & c = static_cast<Composition&>(pop.individual(i));
            std::string file("/Users/makane/code/nynex/output/gen");
            file.append(stringFrom(e.getGA().generation()) + "i" + stringFrom(i) + ".mp3");
            c.bounceToFile(file);
            std::cout << c.serialize() << std::endl;
//            system((std::string("open -a quicktime\\ player ") + file).c_str());
            std::cout << "Enter your numerical opinion: " << std::endl;
            double rating = random() % 5;
            r.addRating(c.getObjectId(), rating);
            std::cout << "Enter someone else's numerical opinion: " << std::endl;
            rating = random() % 5;
            r.addRating(c.getObjectId(), rating);
        }
        e.stepGA();
    }
    
    return 0;
}