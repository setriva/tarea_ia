##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=mrp
ConfigurationName      :=Debug
WorkspacePath          := "/home/nanairo/tarea_ia"
ProjectPath            := "/home/nanairo/tarea_ia"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=nanairo
Date                   :=01/07/15
CodeLitePath           :="/home/nanairo/.codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="mrp.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall -std=c++11 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/aux.cpp$(ObjectSuffix) $(IntermediateDirectory)/servicio.cpp$(ObjectSuffix) $(IntermediateDirectory)/maquina.cpp$(ObjectSuffix) $(IntermediateDirectory)/proceso.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nanairo/tarea_ia/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM "main.cpp"

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) "main.cpp"

$(IntermediateDirectory)/aux.cpp$(ObjectSuffix): aux.cpp $(IntermediateDirectory)/aux.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nanairo/tarea_ia/aux.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/aux.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/aux.cpp$(DependSuffix): aux.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/aux.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/aux.cpp$(DependSuffix) -MM "aux.cpp"

$(IntermediateDirectory)/aux.cpp$(PreprocessSuffix): aux.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/aux.cpp$(PreprocessSuffix) "aux.cpp"

$(IntermediateDirectory)/servicio.cpp$(ObjectSuffix): servicio.cpp $(IntermediateDirectory)/servicio.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nanairo/tarea_ia/servicio.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/servicio.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/servicio.cpp$(DependSuffix): servicio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/servicio.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/servicio.cpp$(DependSuffix) -MM "servicio.cpp"

$(IntermediateDirectory)/servicio.cpp$(PreprocessSuffix): servicio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/servicio.cpp$(PreprocessSuffix) "servicio.cpp"

$(IntermediateDirectory)/maquina.cpp$(ObjectSuffix): maquina.cpp $(IntermediateDirectory)/maquina.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nanairo/tarea_ia/maquina.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/maquina.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/maquina.cpp$(DependSuffix): maquina.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/maquina.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/maquina.cpp$(DependSuffix) -MM "maquina.cpp"

$(IntermediateDirectory)/maquina.cpp$(PreprocessSuffix): maquina.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/maquina.cpp$(PreprocessSuffix) "maquina.cpp"

$(IntermediateDirectory)/proceso.cpp$(ObjectSuffix): proceso.cpp $(IntermediateDirectory)/proceso.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nanairo/tarea_ia/proceso.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/proceso.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/proceso.cpp$(DependSuffix): proceso.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/proceso.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/proceso.cpp$(DependSuffix) -MM "proceso.cpp"

$(IntermediateDirectory)/proceso.cpp$(PreprocessSuffix): proceso.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/proceso.cpp$(PreprocessSuffix) "proceso.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


