  CLS
  LD V8, #4
  LD DT, V8
  LD V3, K
  LD V1, #0
  LD V2, #0
  CALL #230
  LD V9, DT
  SE V9, #0
  JP #20E
  LD V3, K
  LD V1, #0
  LD V2, #5
  CALL #230
  LD V3, K
  LD V1, #0
  LD V2, #10
  CALL #230
  LD V3, K
  LD V1, #0
  LD V2, #15
  CALL #230
  LD V5, K
  JP #200
  LD F, V3
  DRW V1, V2, #5
  RET
