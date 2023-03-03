//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

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
int mask;
uint8_t index;
uint8_t prediction ;
uint8_t *pht; 
//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Bit masking:

int bitmask ( int mask ) {
  int bmask =0;
  for(int i=0; i<mask; ++i) 
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
  int pht_size = 1<<ghistoryBits;
  mask = bitmask( ghistoryBits );
  globalhistory = NOTTAKEN;

  if (bpType == GSHARE ){
    pht = (uint8_t*) malloc(sizeof(uint8_t)*pht_size); // creates pht of  size 2^(ghistoryBits). 
    for (int i=0; i<pht_size ; ++i ) pht[i] = WN;      // and initialised to Weakly not taken. 
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
  uint8_t pc_mask = pc & mask;
  uint8_t gh_mask = globalhistory & mask;

  index = pc_mask ^ gh_mask;

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE: return (pht[index] > 1)?  TAKEN :  NOTTAKEN; 
    case TOURNAMENT:
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
prediction = make_prediction (pc);

  if (outcome==TAKEN){
    globalhistory =globalhistory<<1 | 1;
    if(pht[index] <3) ++pht[index];
    } 
  
  else {
    globalhistory =globalhistory<<1 | 0;
    if(pht[index] >0) --pht[index];
    } 


}
