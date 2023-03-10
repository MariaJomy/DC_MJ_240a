//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

void train_gshare(uint32_t pc, uint8_t outcome);
void train_tournament(uint32_t pc, uint8_t outcome);
// Definitions for 2-bit counters
#define SG  0			// predict G, strong Global
#define WG  1			// predict G, weak Global
#define WL  2			// predict T, weak Local
#define SL  3			// predict T, strong Local

#define GLOBAL    0
#define LOCAL     1

//
// TODO:Student Information
//
const char *studentName = "MARIA JOMY";
const char *studentID   = "PID:A59009603";
const char *email       = "EMAIL:mjomy@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint32_t globalhistory;
uint32_t localhistory;
int g_mask;
int l_mask;
uint8_t index;
uint8_t prediction ;
uint8_t *g_pht;
uint8_t *c_pht;
uint8_t *l_pht;
uint32_t *l_ht;uint8_t gh_mask;
int l_pht_size,  l_ht_size, g_pht_size ;
  uint8_t g_pred,l_pred, c_pred ;
//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Bit masking:

int bitmask ( int g_mask ) {
  int bmask =0;
  for(int i=0; i<g_mask; ++i) 
    bmask|= 1<<i;
  return bmask;

}


// Initialize the predictor
//
void 
init_predictor()
{
  //
  // Initialize Branch Predictor Data Structures
  //
  g_pht_size = 1<<ghistoryBits;
  g_mask = bitmask( ghistoryBits );
  l_pht_size = 1<<lhistoryBits;
  l_ht_size  = 1<<pcIndexBits;

  l_mask = bitmask( lhistoryBits );

  globalhistory = NOTTAKEN;

  if (bpType == GSHARE ){
    g_pht = (uint8_t*) malloc(sizeof(uint8_t)*g_pht_size); // creates g_pht of  size 2^(ghistoryBits). 
    for (int i=0; i<g_pht_size ; ++i ) g_pht[i] = WN;      // and initialised to Weakly not taken. 
  }
  else if(bpType == TOURNAMENT){
    l_ht  = (uint32_t*) malloc(sizeof(uint32_t)*l_ht_size); // creates l_ht  of  size 2^(lhistoryBits).
    l_pht = (uint8_t*) malloc(sizeof(uint8_t)*l_pht_size);  // creates l_pht of  size 2^(lhistoryBits).
    g_pht = (uint8_t*) malloc(sizeof(uint8_t)*g_pht_size);  // creates g_pht of  size 2^(ghistoryBits). 
    c_pht = (uint8_t*) malloc(sizeof(uint8_t)*g_pht_size);  // creates g_pht of  size 2^(ghistoryBits). 
    for (int i=0; i<g_pht_size ; ++i ) g_pht[i] = WN;       // and initialised to Weakly not taken. 
    for (int i=0; i<l_pht_size ; ++i ) l_pht[i] = WN;       // and initialised to Weakly not taken. 
    //Initializing Local History Table
    for (int i=0; i<l_ht_size  ; ++i ) l_ht[i] = NOTTAKEN;      
    for (int i=0; i<g_pht_size ; ++i ) c_pht[i] = WN;       // and initialised to Weakly not taken. 
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  // Implement prediction scheme
  //
  uint8_t pc_mask = pc & g_mask;
   gh_mask = globalhistory & g_mask;

  index = pc_mask ^ gh_mask;

  // Make a prediction based on the bpType
  g_pred = (g_pht[index] > 1)?  TAKEN :  NOTTAKEN;
  //l_pred = (l_pht[l_ht[pc & l_mask]]>1)?TAKEN:NOTTAKEN;
  //c_pred = (c_pht[gh_mask]>1)?LOCAL:GLOBAL;
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE: return g_pred; 
    case TOURNAMENT:{ 
  l_pred = (l_pht[l_ht[pc & l_mask]]>1)?TAKEN:NOTTAKEN;
  c_pred = (c_pht[gh_mask]>1)?LOCAL:GLOBAL;
  return c_pred?g_pred:l_pred;}
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  // Implement Predictor training
  //
  switch (bpType) {
    case STATIC: break;
    case GSHARE:     train_gshare(     pc, outcome); break; 
    case TOURNAMENT: train_tournament( pc, outcome); break;
    case CUSTOM:
    default:
      break;
  }
}
void train_tournament(uint32_t pc, uint8_t outcome)
{
  prediction = make_prediction (pc);
  if (outcome==TAKEN){
    globalhistory =globalhistory<<1 | 1;
    l_ht[pc & l_mask] =(l_ht[pc & l_mask]<<1| 1) & l_mask;  // change second lmask
    if(g_pht[index] <3) ++g_pht[index];
    if(l_pht[l_ht[pc & l_mask]]<3) ++l_pht[l_ht[pc & l_mask]];
    } 
  else {
    globalhistory =globalhistory<<1 | 0;
    if(g_pht[index] >0) --g_pht[index];
    l_ht[pc & l_mask] =(l_ht[pc & l_mask]<<1| 0) & l_mask;
    if(l_pht[l_ht[pc & l_mask]]>0) --l_pht[l_ht[pc & l_mask]];
    } 
  if(l_pred != g_pred){
    if(l_pred == outcome) 
      if(c_pht[gh_mask] <3) ++c_pht[gh_mask];
    if(g_pred == outcome) 
      if(c_pht[gh_mask] >0) --c_pht[gh_mask];

  } 
}

void train_gshare(uint32_t pc, uint8_t outcome)
{
  //
  // Implement Predictor training
  //
prediction = make_prediction (pc);

  if (outcome==TAKEN){
    globalhistory =globalhistory<<1 | 1;
    if(g_pht[index] <3) ++g_pht[index];
    } 
  
  else {
    globalhistory =globalhistory<<1 | 0;
    if(g_pht[index] >0) --g_pht[index];
    } 


}
