# How to run :
# Pure native binary : 			make clean && make
# Untrusted app and sgx lib : 	make clean && make sgx=1
# -----

CC 			:= gcc
CXX 		:= g++
# Only for C builder
CFLAGS 		:= -std=c11
# Only for C++ builder
CXXFLAGS	:= -std=c++11

BUILD_DIR	?= build
SRC_DIRS 	?= src
LIB_DIR 	?= lib

# The package name
PACKAGE_NAME 		:= brahms
# Hard-coded in Dockerfile, do not change
BINARY				:= brahms

SGX	?= 0
ifeq ($(SGX), 0)
  # for Native : not uapp, not test, not _{t,u}
  SRCS := $(shell find $(SRC_DIRS) ! -name '*_t\.c' ! -name '*_u\.c' ! -name '*uapp*' ! -name '*_test*' \( -name '*.cpp' -or -name '*.c' \) )
else
  # for Untrusted App : not test, not API_, not _{t,u}
  SRCS := $(shell find $(SRC_DIRS) ! -name '*_t\.c' ! -name '*_u\.c' ! -name '*_test*' ! -name '*API_*' \( -name '*.cpp' -or -name '*.c' \) )
endif

# Native make
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d) $(shell find $(INC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I$(LIB_DIR)
LDFLAGS := -pthread
# For C or C++ build
CPPFLAGS := $(INC_FLAGS) -MMD -MP -g -Wall -Wextra -fPIC -g1 -lSegFault -Wl,--no-as-needed

# Let the code know what's built : (uapp, tlib) vs. (native)
UApp_CPPFLAGS := $(CPPFLAGS) -DNATIVE -DUAPP
TLib_CPPFLAGS := $(CPPFLAGS) -DENCLAVE
Native_CPPFLAGS := $(CPPFLAGS) -Wpedantic -DNATIVE

# Docker Stuff

ifeq ($(SGX), 0)
# Project Build Image
BUILDER_DOCKERFILE	:= Dockerfile.builder
BUILDER_IMAGE_NAME 	:= $(PACKAGE_NAME)-builder
# Target Deployed Image
REGISTRY_URL 		:=
DOCKERFILE			:= Dockerfile
IMAGE_NAME 			:= $(REGISTRY_URL)$(PACKAGE_NAME)-img
else
BUILDER_DOCKERFILE	:= Dockerfile.builder.sgx
BUILDER_IMAGE_NAME 	:= $(PACKAGE_NAME)-builder-sgx
REGISTRY_URL 		:=
DOCKERFILE			:= Dockerfile.sgx
IMAGE_NAME 			:= $(REGISTRY_URL)$(PACKAGE_NAME)-img-sgx
endif

# -----------------------------------------------------------------------------
.PHONY: all build
all:  build

#  for sgx build untrusted app and trusted lib. for native build only app.
ifeq ($(SGX), 1)
build: $(BUILD_DIR)/$(BINARY) $(BUILD_DIR)/$(SGX_TARGET).signed.so
else
build: $(BUILD_DIR)/$(BINARY)
endif

shared: $(BUILD_DIR)/$(BINARY).so

static: $(BUILD_DIR)/lib$(BINARY).a

# Shared library
# -fPIC: Position Independent Code
$(BUILD_DIR)/$(BINARY).so: $(OBJS)
	$(CXX) -shared -o $@ $(OBJS) $(LDFLAGS)

$(BUILD_DIR)/lib$(BINARY).a: $(OBJS)
	ar -cvq $@ $(OBJS)

# -----------------------------------------------------------------------------
# Testing (not ported to sgx build)
TEST_EXEC := rps-cpp_test
#INC_TEST := $(INC_FLAGS) -Ilib/catch
#LD_TEST := -pthread
SRCS_TEST := $(shell find $(SRC_DIRS) ! -name *uapp* ! -name main.cpp -name *.cpp -or -name *.c)
OBJS_TEST := $(SRCS_TEST:%=$(BUILD_DIR)/%.o)

.PHONY: test
test: $(BUILD_DIR)/$(TEST_EXEC)
# -----------------------------------------------------------------------------

ifeq ($(SGX), 1)
# SGX PROJECT BUILD (First : Untrusted App, Second : Trusted Lib)
# -----------------------------------------------------------------------------
# ######### Untrusted SGX App #########
$(EnclaveSrcDir)/$(EnclaveName)_u.c : $(EnclaveSrcDir)/$(EnclaveName).edl $(SGX_EDGER8R)
	@cd $(dir $@) && $(SGX_EDGER8R) --untrusted $(EnclaveName).edl --search-path ../$(dir $<) --search-path $(SGX_SDK)/include
	@echo "EDGER (App)\t=>\t$@"

$(BUILD_DIR)/%.c.o: %.c
	@echo $(SRCS)
	@$(MKDIR_P) $(dir $@)
	@$(CC) $(UApp_CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo "CC (App) <= $<"

$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(UApp_CPPFLAGS) $(CXXFLAGS) $(App_Include_Paths) -c $< -o $@
	@echo "CXX (App) <= $<"

$(EnclaveObjDir)/$(EnclaveName)_u.o : $(EnclaveSrcDir)/$(EnclaveName)_u.c
	@$(MKDIR_P) $(dir $@)
	@$(CC) ${Encl_CFLAGS} ${Encl_Include} $(App_Include_Paths) -c $< -o $@
	@echo "CC (App) <= $<"

$(BUILD_DIR)/$(BINARY): $(EnclaveObjDir)/$(EnclaveName)_u.o $(OBJS)
	@$(CXX) $^ -o $@ $(App_Link_Flags)
	@echo "LINK =>  $@"

# ######### Trusted SGX Lib #########
$(BUILD_DIR)/$(SGX_TARGET).signed.so : %.signed.so : %.so
	$(SGX_ENCLAVE_SIGNER) sign -enclave $< -config $(EnclaveSrcDir)/$(EnclaveName).config.xml -out $@ -key $(EnclaveSrcDir)/enclave-key.pem
	@echo "SIGN (Enclave) => $@"

$(BUILD_DIR)/$(SGX_TARGET).so : $(EnclAllObjs)
	$(CXX) $^ -o $@ $(EnclaveLFlags)
	@echo "LINK (Enclave) => $@"

$(EnclaveSrcDir)/$(EnclaveName)_t.c : $(EnclaveSrcDir)/$(EnclaveName).edl $(SGX_EDGER8R)
	@cd $(dir $@) && $(SGX_EDGER8R) --trusted $(EnclaveName).edl --search-path ../$(dir $<) --search-path $(SGX_SDK)/include
	@echo "EDGER (Enclave) => $@"

$(EnclaveObjDir)/$(EnclaveName)_t.o : $(EnclaveSrcDir)/$(EnclaveName)_t.c
	@${CC} -c $< -o $@ ${Encl_CFLAGS} ${Encl_Include}
	@echo "CC (Enclave) <= $<"

$(EnclaveCXXObjs) : $(EnclaveObjDir)/%_t.o : $(EnclaveSrcDir)/%.cpp
	@${CXX} -c $< -o $@ ${Encl_Include} $(CPPFLAGS) ${Encl_CXXFLAGS}
	@echo "CXX (Enclave) <= $<"

$(TypesCXXObjs) : $(TypesObjDir)/%_t.o : $(TypesSrcDir)/%.cpp
	@${CXX} -c $< -o $@ ${Encl_Include} $(CPPFLAGS) ${Encl_CXXFLAGS}
	@echo "CXX (Enclave) <= $<"

$(OnionCXXObjs) : $(OnionObjDir)/%_t.o : $(OnionSrcDir)/%.cpp
	@${CXX} -c $< -o $@ ${Encl_Include} $(CPPFLAGS) ${Encl_CXXFLAGS}
	@echo "CXX (Enclave) <= $<"

$(CryptoCXXObjs) : $(CryptoObjDir)/%_t.o : $(CryptoSrcDir)/%.cpp
	@${CXX} -c $< -o $@ ${Encl_Include} $(CPPFLAGS) ${Encl_CXXFLAGS}
	@echo "CXX (Enclave) <= $<"

$(MarshalCXXObjs) : $(MarshalObjDir)/%_t.o : $(MarshalSrcDir)/%.cpp
	@${CXX} -c $< -o $@ ${Encl_Include} $(CPPFLAGS) ${Encl_CXXFLAGS}
	@echo "CXX (Enclave) <= $<"

$(UtilsCXXObjs) : $(UtilsObjDir)/%_t.o : $(UtilsSrcDir)/%.cpp
	@${CXX} -c $< -o $@ ${Encl_Include} $(CPPFLAGS) ${Encl_CXXFLAGS}
	@echo "CXX (Enclave) <= $<"
# -----------------------------------------------------------------------------
else
# NATIVE BUILD (just native app, no sgx)
# -----------------------------------------------------------------------------
$(BUILD_DIR)/$(BINARY): $(OBJS)
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@echo "LINK (Nat) => $@"

$(BUILD_DIR)/$(TEST_EXEC): $(OBJS_TEST)
	@$(CXX) $(OBJS_TEST) -o $@ $(LDFLAGS) $(LD_TEST)
	@echo "LINK (Nat) => $@"

$(BUILD_DIR)/%.c.o: %.c
	@$(MKDIR_P) $(dir $@)
	@$(CC) $(Native_CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo "CC (Nat) <= $<"

$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(Native_CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	@echo "CXX (Nat) <= $<"

# -----------------------------------------------------------------------------
endif


.PHONY: clean clean-sgx

clean: clean-sgx
	@$(RM) -r $(BUILD_DIR)/*

clean-sgx:
	@bash -c "rm -rf $(EnclaveSrcDir)/*_{t,u}.{c,h}"

.DELETE_ON_ERROR:

# -----------------------------------------------------------------------------
# Docker

# Create and push the image (IMAGE_NAME needs to be a repo url)
docker-push: docker
	docker push $(IMAGE_NAME)

# Create the project image
docker: docker-build
	docker build -f $(DOCKERFILE) -t $(IMAGE_NAME) $(CURDIR)
	#docker build --no-cache -f $(DOCKERFILE) -t $(IMAGE_NAME) $(CURDIR)

# Builds the project inside the builder image
docker-build:
	docker run --rm -v "$(CURDIR):/src/" $(BUILDER_IMAGE_NAME) \
		make build BUILD_DIR=$(BUILD_DIR)/docker SGX=$(SGX)

# Launch this rule if you modify Dockerfile.builder
docker-builder:
	docker build -f $(BUILDER_DOCKERFILE) -t $(BUILDER_IMAGE_NAME) \
		$(CURDIR)

# -----------------------------------------------------------------------------

-include $(DEPS)

MKDIR_P ?= mkdir -p
