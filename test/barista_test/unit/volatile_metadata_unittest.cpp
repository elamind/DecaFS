#include "../../../src/app/barista_core/barista_core.h"
#include "gtest/gtest.h"

#define VALID_CHUNK_SIZE 2048
#define ALT_VALID_CHUNK_SIZE 4096
#define INVALID_CHUNK_SIZE 64 

#define VALID_STRIPE_SIZE 2048
#define ALT_VALID_STRIPE_SIZE 4096
#define INVALID_STRIPE_SIZE 64

#define BARISTA_NODE_NUM 1
#define ESPRESSO_1_NODE_NUM 2
#define ESPRESSO_2_NODE_NUM 3

const char barista[] = "192.168.1.100";
const char espresso_1[] = "192.168.1.101";
const char espresso_2[] = "192.168.1.102";
const char invalid_ip[] = "0.0.0.0";

TEST (Volatile_Metadata, DefaultConstructor) {
  Volatile_Metadata v_meta;

  EXPECT_EQ (0, v_meta.get_chunk_size());
  EXPECT_EQ (0, v_meta.get_stripe_size());
}

TEST (Volatile_Metadata, SetChunkSizeValid) {
  Volatile_Metadata v_meta;

  v_meta.set_chunk_size (VALID_CHUNK_SIZE);
  EXPECT_EQ (VALID_CHUNK_SIZE, v_meta.get_chunk_size());
}

TEST (Volatile_Metadata, SetChunkSizeInvalid) {
  Volatile_Metadata v_meta;

  v_meta.set_chunk_size (INVALID_CHUNK_SIZE);
  EXPECT_EQ (0, v_meta.get_chunk_size());
}

TEST (Volatile_Metadata, SetStripeSizeValid) {
  Volatile_Metadata v_meta;

  v_meta.set_stripe_size (VALID_STRIPE_SIZE);
  EXPECT_EQ (VALID_STRIPE_SIZE, v_meta.get_stripe_size());
}

TEST (Volatile_Metadata, SetStripeSizeInvalid) {
  Volatile_Metadata v_meta;

  v_meta.set_stripe_size (INVALID_STRIPE_SIZE);
  EXPECT_EQ (0, v_meta.get_stripe_size());
}

TEST (Volatile_Metadata, ResetChunkSize) {
  Volatile_Metadata v_meta;

  v_meta.set_chunk_size (VALID_CHUNK_SIZE);
  EXPECT_EQ (VALID_CHUNK_SIZE, v_meta.get_chunk_size());
  v_meta.set_chunk_size (ALT_VALID_CHUNK_SIZE);
  EXPECT_EQ (VALID_CHUNK_SIZE, v_meta.get_chunk_size());
}

TEST (Volatile_Metadata, ResetStripeSize) {
  Volatile_Metadata v_meta;

  v_meta.set_stripe_size (VALID_STRIPE_SIZE);
  EXPECT_EQ (VALID_STRIPE_SIZE, v_meta.get_stripe_size());
  v_meta.set_stripe_size (ALT_VALID_STRIPE_SIZE);
  EXPECT_EQ (VALID_STRIPE_SIZE, v_meta.get_stripe_size());
}

TEST (Volatile_Metadata, AddNode) {
  Volatile_Metadata v_meta;

  v_meta.add_node ((char *)barista, BARISTA_NODE_NUM);
  EXPECT_EQ (BARISTA_NODE_NUM, v_meta.get_node_number ((char *)barista));
  v_meta.add_node ((char *)espresso_1, ESPRESSO_1_NODE_NUM);
  EXPECT_EQ (ESPRESSO_1_NODE_NUM, v_meta.get_node_number ((char *)espresso_1));
}

TEST (Volatile_Metadata, GetActiveNodeCount) {
  Volatile_Metadata v_meta;

  v_meta.add_node ((char *)barista, BARISTA_NODE_NUM);
  v_meta.add_node ((char *)espresso_1, ESPRESSO_1_NODE_NUM);

  EXPECT_EQ (2, v_meta.get_active_node_count());
}

TEST (Volatile_Metadata, GetActiveNodes) {
  Volatile_Metadata v_meta;

  v_meta.add_node ((char *)barista, BARISTA_NODE_NUM);
  v_meta.add_node ((char *)espresso_1, ESPRESSO_1_NODE_NUM);

  char **nodes = (char **)malloc (1);

  EXPECT_EQ (2, v_meta.get_active_nodes(&nodes));
  ASSERT_STREQ (barista, nodes[0]);
  ASSERT_STREQ (espresso_1, nodes[1]);
}

TEST (Volatile_Metadata, NodeDown) {
  Volatile_Metadata v_meta;

  v_meta.add_node ((char *)barista, BARISTA_NODE_NUM);
  v_meta.add_node ((char *)espresso_1, ESPRESSO_1_NODE_NUM);
  v_meta.add_node ((char *)espresso_2, ESPRESSO_2_NODE_NUM);
  
  char **nodes = (char **)malloc (1);

  EXPECT_EQ (3, v_meta.get_active_nodes(&nodes));
  v_meta.set_node_down ((char *)espresso_1);

  EXPECT_EQ (2, v_meta.get_active_nodes(&nodes));
  ASSERT_STREQ (barista, nodes[0]);
  ASSERT_STREQ (espresso_2, nodes[2]);
}

TEST (Volatile_Metadata, NodeDownDoesNotExist) {
  Volatile_Metadata v_meta;

  v_meta.add_node ((char *)barista, BARISTA_NODE_NUM);
  v_meta.add_node ((char *)espresso_1, ESPRESSO_1_NODE_NUM);
  v_meta.add_node ((char *)espresso_2, ESPRESSO_2_NODE_NUM);
  
  char **nodes = (char **)malloc (1);
  
  EXPECT_EQ (3, v_meta.get_active_nodes(&nodes));
  v_meta.set_node_down ((char *)invalid_ip);

  EXPECT_EQ (3, v_meta.get_active_nodes(&nodes));
}

TEST (Volatile_Metadata, NodeUp) {
  Volatile_Metadata v_meta;

  v_meta.add_node ((char *)barista, BARISTA_NODE_NUM);
  v_meta.add_node ((char *)espresso_1, ESPRESSO_1_NODE_NUM);
  v_meta.add_node ((char *)espresso_2, ESPRESSO_2_NODE_NUM);
  
  char **nodes = (char **)malloc (1);

  EXPECT_EQ (3, v_meta.get_active_nodes(&nodes));
  
  v_meta.set_node_down ((char *)espresso_1);
  EXPECT_EQ (2, v_meta.get_active_nodes(&nodes));
 
  v_meta.set_node_up ((char *)espresso_1);
  EXPECT_EQ (3, v_meta.get_active_nodes(&nodes));
  
}

TEST (Volatile_Metadata, NodeUpDoesNotExist) {
  Volatile_Metadata v_meta;

  v_meta.add_node ((char *)barista, BARISTA_NODE_NUM);
  v_meta.add_node ((char *)espresso_1, ESPRESSO_1_NODE_NUM);
  v_meta.add_node ((char *)espresso_2, ESPRESSO_2_NODE_NUM);
  
  char **nodes = (char **)malloc (1);
  
  EXPECT_EQ (3, v_meta.get_active_nodes(&nodes));
  
  v_meta.set_node_up ((char *)invalid_ip);

  EXPECT_EQ (3, v_meta.get_active_nodes(&nodes));
}