#include "../redismodule.h"
#include "../rmutil/util.h"
#include "../rmutil/strings.h"
#include "../rmutil/test_util.h"


int ExecCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

  if (argc != 2) {
    return RedisModule_WrongArity(ctx);
  }
  RedisModule_AutoMemory(ctx);

  size_t cmd_len;
  char *cmd = RedisModule_StringPtrLen(argv[1], &cmd_len);

  FILE *fp = popen(cmd, "r");
  char buf[1024] = {0}, output[10240] = {0};

  while (fgets(buf, sizeof(buf), fp) != 0) {
      strcat(output, buf);
  }

  RedisModuleString *ret = RedisModule_CreateString(ctx, output, strlen(output));
  RedisModule_ReplyWithString(ctx, ret);
  pclose(fp);
  return REDISMODULE_OK;
}


int RedisModule_OnLoad(RedisModuleCtx *ctx) {

  if (RedisModule_Init(ctx, "system", 1, REDISMODULE_APIVER_1) ==
      REDISMODULE_ERR) {
    return REDISMODULE_ERR;
  }

  if (RedisModule_CreateCommand(ctx, "system.exec", ExecCommand, "readonly",
                                1, 1, 1) == REDISMODULE_ERR) {
    return REDISMODULE_ERR;
  }

  return REDISMODULE_OK;
}
