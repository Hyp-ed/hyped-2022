
# Libaries for generating TARGET
EIGEN     := $(LIBS_DIR)/Eigen
RAPIDJSON := $(LIBS_DIR)/rapidjson
GITHOOKS  := .git/hooks

DEPENDENCIES  := $(EIGEN) $(RAPIDJSON) $(GITHOOKS)

# Re-install eigen library if the tar file changes
# EIGEN is a header only lib, no compilation needed
$(EIGEN): $(EIGEN).tar.gz
	$(Echo) Unpacking Eigen library $@
	$(Verb) tar -zxvf $@.tar.gz -C $(LIBS_DIR) > /dev/null
	$(Verb) touch $@

# Re-install rapidjson library if the tar file changes
# RAPIDJSON is a header only lib, no compilation needed
$(RAPIDJSON): $(RAPIDJSON).tar.gz
	$(Echo) Unpacking RapidJSON library $@
	$(Verb) tar -zxvf $@.tar.gz -C $(LIBS_DIR) > /dev/null
	$(Verb) touch $@


$(GITHOOKS): utils/githooks/*
	$(Echo) New githooks, installing
	$(Verb) ./setup.sh
