  CLS
  JP #210
  LD F, V3
  DRW V1, V2, #5
  RET
  LD V3, #0
  LD V1, #0
  LD V2, #0
  CALL #204
  LD V3, #1
  LD V1, #0
  LD V2, #5
  CALL #204
  LD V3, #2
  LD V1, #0
  LD V2, #10
  CALL #204
  LD V3, #3
  LD V1, #0
  LD V2, #15
  CALL #204
  JP #200
