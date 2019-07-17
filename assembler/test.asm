      CLS
      JP main

main: LD V3, K
      LD V1, #0
      LD V2, #0
      CALL draw
      LD V3, K
      LD V1, #6
      LD V2, #0
      CALL draw
      LD V3, K
      LD V1, #c
      LD V2, #0
      CALL draw
      LD V3, K
      LD V1, #12
      LD V2, #0
      CALL draw
      JP main

draw: LD F, V3
      DRW V1, V2, #5
      RET
