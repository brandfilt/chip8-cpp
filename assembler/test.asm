  CLS
  LD V10, #4
  LD DT, V10
  LD V3, #0
  LD V1, #0
  LD V2, #0
  CALL #232
  LD V11, DT
  SE V11, #3
  JP #210
  LD V3, #1
  LD V1, #0
  LD V2, #5
  CALL #232
  LD V3, #2
  LD V1, #0
  LD V2, #10
  CALL #232
  LD V3, #3
  LD V1, #0
  LD V2, #15
  CALL #232
  LD V5, K
  JP #200
  LD F, V3
  DRW V1, V2, #5
  RET
