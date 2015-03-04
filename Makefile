DDATA		= ./data
DHELPERS	= ./helpers
LINKOPTS 	= -L../lib -lEasyLocalpp
CXXFLAGS	= -O2 -Wall
CXXFLAGS	+= -g3 -ggdb
CXX			= g++
_OBJ		= vrp_state_manager.o vrp_neighborhood_explorer.o \
			  vrp_output_manager.o vrp_tabu_list_manager.o
OBJ 		= $(patsubst %, $(DHELPERS)/%, $(_OBJ))
all: vrptw

vrptw: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -o $@ $(OBJ) \
	vehicle.o client.o order.o carrier.o move.o route.o \
	prob_input.o billing.o

# DATA
$(DDATA)/client.o: $(DDATA)/client.cc
	$(CXX) $(CXXFLAGS) -c $^ -I.
$(DDATA)/carrier.o: $(DDATA)/carrier.cc
	$(CXX) $(CXXFLAGS) -c $^ -I.
$(DDATA)/order.o: $(DDATA)/order.c
	$(CXX) $(CXXFLAGS) -c $^ -I.
$(DDATA)/vehicle.o: $(DDATA)/vehicle.cc
	$(CXX) $(CXXFLAGS) -c $^ -I.
$(DDATA)move.o: $(DDATA)move.cc
	$(CXX) $(CXXFLAGS) -c $^ -I.
$(DDATA)/route.o: $(DDATA)/route.cc $(DDATA)/prob_input.h
	$(CXX) $(CXXFLAGS) -c $^ -I.
$(DDATA)/prob_input.o: $(DDATA)/prob_input.cc $(DDATA)/carrier.h \
					   $(DDATA)/vehicle.h $(DDATA)/client.h \
					   $(DDATA)/order.h $(DDATA)/carrier.h \
					   $(DHELPERS)/billing_cost_component.h \
	$(CXX) $(CXXFLAGS) -c $^ -lm -I.
$(DDATA)/billing.o: $(DDATA)/billing.cc $(DHELPERS)/billing_cost_component.h
	$(CXX) $(CXXFLAGS) -c $^ -lm -I.

# HELPERS
$(DHELPERS)/billing_cost_component.o: $(DHELPERS)/billing_cost_component.cc \
									  $(DDATA)/prob_input.h $(DDATA)/billing.h
	$(CXX) $(CXXFLAGS) -c $^ -lm -I.
$(DHELPERS)/vrp_state_manager.o: $(DHELPERS)/vrp_state_manager.cc \
								 $(DDATA)/prob_input.h $(DDATA)/route.h \
								 $(DDATA)/order.h $(DDATA)/billing.h \
								 $(DHELPERS)/billing_cost_component.h
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -c $^ -I.
$(DHELPERS)/vrp_neighborhood_explorer.o: $(DHELPERS)/vrp_neighborhood_explorer.cc \
										 $(DHELPERS)/vrp_state_manager.h \
										 $(DHELPERS)/billing_cost_component.h \
										 $(DDATA)/route.h $(DDATA)/move.h \
										 $(DDATA)/prob_input.h $(DDATA)/billing.h
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -c $^ -I.
$(DHELPERS)/vrp_tabu_list_manager.o: $(DHELPERS)/vrp_tabu_list_manager.cc \
									 $(DDATA)/route.h $(DDATA)/move.h
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -c $^ -I.
$(DHELPERS)/vrp_output_manager.o: $(DHELPERS)/vrp_output_manager.h \
								  $(DDATA)/prob_input.h $(DDATA)/route.h
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -c $^ -I.

.PHONY: clean
clean:
	@rm $(DDATA)/*.o $(DHELPERS)/*.o main
