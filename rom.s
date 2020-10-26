lbl0:
    BRK
    LDA #$10

    .org $8000

reset:
    LDA #$10

    .org $fffc
    .word reset
    .word $0000
