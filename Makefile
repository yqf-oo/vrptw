DDATA		= ./data
DHELPERS	= ./helpers
DSOLVERS    = ./solvers
LINKOPTS 	= -L../lib -lEasyLocalpp -lstdc++ -I../easylocal-2/src
CXXFLAGS	= -O2 -Wall
CXXFLAGS	+= -g3 -std=c++11 -ggdb
CXX			= g++
_HOBJ		= vrp_state_manager.o vrp_neighborhood_explorer.o \
			  vrp_tabu_list_manager.o billing_cost_component.o
HELPER_OBJ 		= $(patsubst %, $(DHELPERS)/%, $(_HOBJ))
_DOBJ		= vehicle.o client.o order.o \
			  neighbor.o prob_input.o route.o billing.o
DATA_OBJ 	= $(patsubst %, $(DDATA)/%, $(_DOBJ))
SOLVER_OBJ  = $(DSOLVERS)/vrp_token_ring_search.o \
			  $(DSOLVERS)/vrp_token_ring_observer.o
all: vrptw

vrptw: main.o $(HELPER_OBJ) $(SOLVER_OBJ) $(DATA_OBJ)
	/usr/bin/libtool --tag=CXX --mode=link $(CXX) $(CXXFLAGS) $(LINKOPTS) -o $@ $^
main.o: main.cc
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -o $@ -c $< -I.
# DATA
$(DDATA)/client.o: $(DDATA)/client.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^ -I.
# $(DDATA)/carrier.o: $(DDATA)/carrier.h
# 	$(CXX) $(CXXFLAGS) -o $@ -c $^ -I.
$(DDATA)/order.o: $(DDATA)/order.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^ -I.
$(DDATA)/vehicle.o: $(DDATA)/vehicle.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^ -I.
$(DDATA)/neighbor.o: $(DDATA)/neighbor.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^ -I.

$(DDATA)/route.o: $(DDATA)/route.cc $(DDATA)/prob_input.h
	$(CXX) $(CXXFLAGS) -o $@ -c $< -I.
$(DDATA)/prob_input.o: $(DDATA)/prob_input.cc $(DDATA)/carrier.h \
					   $(DDATA)/vehicle.h $(DDATA)/client.h \
					   $(DDATA)/order.h $(DDATA)/carrier.h \
					   $(DHELPERS)/billing_cost_component.h
	$(CXX) $(CXXFLAGS) -o $@ -c $< -lm -I.
$(DDATA)/billing.o: $(DDATA)/billing.cc $(DHELPERS)/billing_cost_component.h
	$(CXX) $(CXXFLAGS) -o $@ -c $< -lm -I.

# HELPERS
$(DHELPERS)/billing_cost_component.o: $(DHELPERS)/billing_cost_component.cc \
									  $(DDATA)/prob_input.h $(DDATA)/billing.h
	$(CXX) $(CXXFLAGS) -o $@ -c $< -lm -I.
$(DHELPERS)/vrp_state_manager.o: $(DHELPERS)/vrp_state_manager.cc \
								 $(DDATA)/prob_input.h $(DDATA)/route.h \
								 $(DDATA)/order.h $(DDATA)/billing.h \
								 $(DHELPERS)/billing_cost_component.h
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -o $@ -c $< -I.
$(DHELPERS)/vrp_neighborhood_explorer.o: $(DHELPERS)/vrp_neighborhood_explorer.cc \
										 $(DHELPERS)/vrp_state_manager.h \
										 $(DHELPERS)/billing_cost_component.h \
										 $(DDATA)/route.h $(DDATA)/neighbor.h \
										 $(DDATA)/prob_input.h $(DDATA)/billing.h
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -o $@ -c $< -I.
$(DHELPERS)/vrp_tabu_list_manager.o: $(DHELPERS)/vrp_tabu_list_manager.cc \
									 $(DDATA)/route.h $(DDATA)/neighbor.h
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -o $@ -c $< -I.
# $(DHELPERS)/vrp_output_manager.o: $(DHELPERS)/vrp_output_manager.h
# 								  $(DDATA)/prob_input.h $(DDATA)/route.h
# 	$(CXX) $(CXXFLAGS) $(LINKOPTS) -o $@ -c $< -I.

# SOLVERS
$(DSOLVERS)/%.o: $(DSOLVERS)/%.cc
	$(CXX) $(CXXFLAGS) $(LINKOPTS) -o $@ -c $< -I.

.PHONY: clean
clean:
	@rm -f $(DDATA)/*.o $(DHELPERS)/*.o solvers/*.o main.o
