    ;; Jump to main
        JP main
    ;; Sprite data
%wh     1000000
%bl     1000000

    ;; Check if pixel is white
    ;; V0 and V1 have the coordinate
    ;; Result is stored in V2
check:  LD I, wh
        DRW V0, V1, #1
        LD V2, Vf
        SE Vf, #1
        RET
        LD I, bl
        DRW V0, V1, #1
        RET


main:
