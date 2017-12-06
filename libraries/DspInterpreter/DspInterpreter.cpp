
#include "DspInterpreter.h"


//---- Commands class ----

DESP_Commands::DESP_Commands() {
  cmd=0; 
  isFunctionIF=NULL;
}

void DESP_Commands::init() {

}


void DESP_Commands::first(){
	if (cmd!=0) 
   while (cmd->previous!=0) 
     cmd=cmd->previous; 
}

void DESP_Commands::last(){
  if (cmd!=0) 
   while (cmd->next!=0) 
     cmd=cmd->next; 
}

void DESP_Commands::next(){
	if (cmd!=0) 
   if (cmd->next!=0) cmd=cmd->next;  

 // Serial.print("CMD:");Serial.println(int(cmd),DEC);

}

struct DspCommand * DESP_Commands::newCommand(){
  struct DspCommand *temp;

  temp = new DspCommand; 
  temp->var1idx=NOVAR;   
  temp->var2idx=NOVAR;
  temp->previous=0;
  temp->next=0;
  temp->containerCmds=0;
  temp->elseCmds=0;

  if (cmd != 0) { //link to the list
    last();//goto last command to link the new one
    temp->previous=cmd;
    cmd->next=temp;
  }
  cmd=temp;
  
  return cmd;
}

void DESP_Commands::deleteCommand(){
  struct DspCommand* pre,*nxt;  

  pre=cmd->previous;
  nxt=cmd->next;
  if (cmd->containerCmds!=0) {
    cmd->containerCmds->deleteAll();
    delete cmd->containerCmds;
  }  
  if (pre!=0) pre->next=nxt;
  if (nxt!=0) nxt->previous=pre;
  delete cmd;  
  cmd=pre;
}

void DESP_Commands::deleteAll(){
	
  last();
  while (!isEmpty()) 
    deleteCommand();
  cmd=0;
}

boolean DESP_Commands::isEmpty(){
  return cmd==0;   
}

boolean DESP_Commands::isVarValid(int8_t varidx){
	return varidx!=NOVAR;
}


boolean DESP_Commands::isContainerCommand(){
  switch (cmd->cmdid){
    //container Commands
    //TODO: add user procedures or functions
    //       return true;
   
   default:
           return isLoopCommand() || isControlCommand();//All control commands (i.e. IFs) and loops are containers
  }
  
}

boolean DESP_Commands::isLoopCommand(){
  switch (cmd->cmdid){
    
   case 70:
   case 72:
           return true;
   
   default:
           return false;
  }
  
}

boolean DESP_Commands::isControlCommand(){ //IFs
	
	if (isFunctionIF!=NULL)
		return isFunctionIF(cmd->cmdid); //Let the user set the control commands
	
  switch (cmd->cmdid){
    
   case 94:  //check digitalpin is high
   case 95:	 //check digitalpin is low
   case 136: //IR distance sensor	
   case 141: //Usonic Device if distance greater,etc
   case 146: //Switch device
   case 166: //Moisture
   case 171: //Temp GT,etc	
   case 173: //Humid GT,etc
   case 177: //pressure
   case 178: //relative pressure
   case 179: //temp from bmp180		
           return true;
   
   default:
           return false;
  }
  
}


boolean DESP_Commands::isContainerEndCommand(int id){
  if (id==0) id=cmd->cmdid;
  
  switch (id){
    
   case 99: //end container command
   	      // Serial.println(" END CMD ");
           return true;
   
   default:
           return false;
  }
  
}

boolean DESP_Commands::isControlElseCommand(int id){
  if (id==0) id=cmd->cmdid;
  
  switch (id){
    
   case 100: //else command
           return true;
   
   default:
           return false;
  }
  
}

boolean DESP_Commands::isStringCommand(int id){
  if (id==0) id=cmd->cmdid;
  
  switch (id){
    
   case 121://LCDprint
   case 124:	
   case 125:
           return true;
   
   default:
           return false;
  }
  
}


// --- stack class  ----

DESP_Stack::DESP_Stack(){
  cur=-1;
  
}

void DESP_Stack::push(DESP_Commands *cmds){
  myStack[++cur]=cmds;    
}

struct DESP_Commands * DESP_Stack::pop(){
 if ((cur>-1)  and (cur<STACKSIZE)){
  return myStack[cur--];
 }
 else
  return 0;
  
}

//---- Interpreter class ----

DESP_Interpreter::DESP_Interpreter() {
  running=false;
  evaluate=0;
  isFunctionIF=NULL;
  program= new DESP_Commands; //create the main commands class
//  stack= new DESP_Stack; 
}

void DESP_Interpreter::newProgram() {
  deleteProgram();
  running=false;
}

void DESP_Interpreter::deleteProgram() {
  if (!program->isEmpty()) program->deleteAll();
}

void DESP_Interpreter::init() {

}

void DESP_Interpreter::setfuncif(isfuncif myfunc){
	isFunctionIF=myfunc; //save it for later
	program->isFunctionIF=myfunc;
}

void DESP_Interpreter::startProgram() {
	if (evaluate!=0){
   reset();
   program->first();
   running=true;  
 }
}

struct DspCommand* DESP_Interpreter::getCommandToExecute() {//done: commands could be containers should take that into account
  if (running){  	 
  	  if (program->isContainerCommand()){
                
      //while (program->isContainerCommand() and !program->cmd->containerCmds->isEmpty() and evaluate(program->cmd)){//was while
        if (evaluate(program->cmd)){//evaluate
         if (!program->cmd->containerCmds->isEmpty()){//there are commands on main container
          stack.push(program);
          program->loopcommand=program->cmd;
          program->loopcounter=0;
          program=program->cmd->containerCmds;    
          program->first();           	
         } 	
        }
        else{//the else clause here
         if ((program->cmd->elseCmds!=0) && !program->cmd->elseCmds->isEmpty()){//there are commands on else container
         	//Serial.println("Else commands ok");
          stack.push(program);
          program->loopcommand=program->cmd;
          program->loopcounter=0;
          program=program->cmd->elseCmds;    
          program->first(); 
         }  else {
         	  //Serial.println("No Else clause");
         	  return program->cmd;
         	}
         	        	        	
        }
        
      }//is container
      if (program->isContainerCommand() and (!program->cmd->containerCmds->isEmpty())) // evaluate next container command
      	return getCommandToExecute();
      else 	
        return program->cmd;
  }
  else
    return 0;  
}

void DESP_Interpreter::commandExecuted(){
DESP_Commands* t;  
 
 // Serial.print(" ,CMDEXEC ");Serial.println(program->cmd->cmdid); 
  if (!running) return;
 
  if (program->cmd->next!=0)
    program->next();
  else //no more commands
  {
    t=stack.pop();
    if (t>0){ //container commands are over
      program=t; 
      if (++program->loopcounter!=program->loopcommand->loopcnt){ //repeat loop
      	if (program->cmd->cmdid==72)//infinite loop so dec counter
      		 --program->loopcounter;
      	//Serial.print(" ,Loop ");	 Serial.println(program->loopcounter);
        stack.push(program);    
        program=program->cmd->containerCmds;    
        program->first();      
      }  
      else { program->next();//loopended go forward       
      	//Serial.print(" ,Loop END ");
      }
    }
    else //program commands are over
    	{ 
     running=false;  
     Serial.println(" Program ENDED ");
   }
    
  }
  
}

void DESP_Interpreter::reset(){
  DESP_Commands* t;

 t=stack.pop();
 while (t!=0){
  program=t; 
//  Serial.println(program,DEC);
  t=stack.pop();  
 }
  
}

void DESP_Interpreter::endProgram() {
   
 reset();
 running=false; 
}


struct DspCommand* DESP_Interpreter::addCommand(int id,int par1,int par2,int devid){

  if (program->isContainerEndCommand(id) || program->isControlElseCommand(id) ){
    program=stack.pop();
  } 
  
   if (!program->isControlElseCommand(id)){//else command is not added at all
     program->newCommand();
     program->cmd->cmdid=id;
     program->cmd->devid=devid;
     program->cmd->param1=par1; 
     program->cmd->param2=par2; 
     program->cmd->paramstr=NULL;     
   }
  if (program->isContainerCommand()||program->isControlElseCommand(id)){
  	if (program->isLoopCommand())
  	   program->cmd->loopcnt=par1;
     else program->cmd->loopcnt=1;
    	
    
    stack.push(program);
    if (program->isControlElseCommand(id)){//else command adds the commands to the else container
     program->cmd->elseCmds= new DESP_Commands;//create a command list class for the esle clause all commands that follow goes in here until container ended code 99 or 100 =else
     program=program->cmd->elseCmds;    
     program->isFunctionIF=isFunctionIF;
   }
    else{
     program->cmd->containerCmds= new DESP_Commands;//create a command list class for the container all commands that follow goes in here until container ended code 99 or 100 =else
     program=program->cmd->containerCmds;
     program->isFunctionIF=isFunctionIF;    
   }
  }
   
  return program->cmd; 
  
}

boolean DESP_Interpreter::isStringCommand(int id){
	
	return program->isStringCommand(id);
}

int DESP_Interpreter::getParam1(DspCommand* vcmd){
	if (vcmd!=0) {
		if (program->isVarValid(vcmd->var1idx)) return getVar(vcmd->var1idx);
		else return vcmd->param1;					
	}
}
int DESP_Interpreter::getParam2(DspCommand* vcmd){
	if (vcmd!=0) {
		if (program->isVarValid(vcmd->var2idx)) return getVar(vcmd->var2idx);
		else return vcmd->param2;					
	}	
}

void DESP_Interpreter::setVar(int8_t vidx,int value){	
 arduVars[vidx]=value;	
}
int DESP_Interpreter::getVar(int8_t vidx){
	if (vidx>74){
		 if (evalvar!=0)
		 	return evalvar(program->cmd,vidx); 
		 else	
		  return 0;
	}
	else	
	 return arduVars[vidx];	
}
