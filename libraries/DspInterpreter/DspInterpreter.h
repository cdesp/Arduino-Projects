

#ifndef dspinterpreter_h
#define dspinterpreter_h

#define stacksize 20
#define NOVAR -1

#include <arduino.h>




class DESP_Commands;//forwward declaration



typedef struct  DspCommand{
     int cmdid;
     int devid; //Device id -1 for none
     int param1;
     int param2;
     char *paramstr;
     int8_t var1idx; //variable index signed byte so up to 127 vars
     int8_t var2idx; //variable index     
     int loopcnt;
     struct DspCommand *previous;//pointer to previous command
     struct DspCommand *next; //pointer to next command
     DESP_Commands *containerCmds;//pointer to a container commands class
     DESP_Commands *elseCmds;//pointer to a container commands class for the else clause on if command
} DspCommand;

typedef boolean (*function) (DspCommand*) ;
typedef int (*funcevalvar) (DspCommand*,int) ;


class DESP_Stack
{
  DESP_Commands * myStack[stacksize];
  int cur;
  
  public:
    DESP_Stack();
    void push(DESP_Commands *cmds);
    struct DESP_Commands * pop();
  
};

class DESP_Commands //linked list
{
 protected:

 public:
    struct DspCommand *loopcommand;
    int loopcounter;
    struct DspCommand *cmd;//pointer to a command    
    DESP_Commands();
    void init();
    void first();//go to first command
    void last();//go to last command
    void next(); //go to next command
    struct DspCommand * newCommand();
    void deleteCommand();
    void deleteAll();         
    boolean isEmpty();
    boolean isContainerCommand();
    boolean isLoopCommand();
    boolean isControlCommand();    
    boolean isContainerEndCommand(int id);
    boolean isControlElseCommand(int id);
		boolean isStringCommand(int id);    
		boolean isVarValid(int8_t varidx);
};


class DESP_Interpreter
{
 private:
     DESP_Commands* program;//our program    
     DESP_Stack  stack;          
     int arduVars[20];//max 20 integer vars
  public:
     boolean running; //if we are running a program
     function evaluate;
     funcevalvar evalvar;
     DESP_Interpreter();
     void init();
     void newProgram();
     void deleteProgram();      
     void startProgram();
     struct DspCommand* getCommandToExecute();//returns command to be executed TODO:execute an outsite function maybe with params
     void commandExecuted();
     void reset();
     void endProgram();
     struct DspCommand* addCommand(int id,int par1,int par2,int devid=-1);
     boolean isStringCommand(int id);
     int getParam1(DspCommand* vcmd);
     int getParam2(DspCommand* vcmd);
     void setVar(int8_t vidx,int value);
     int getVar(int8_t vidx);
};



#endif

