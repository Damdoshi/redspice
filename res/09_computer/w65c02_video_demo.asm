  ; W65C02 / BunnyCAD video RAM demo
  ; VRAM $0000-$0FFF: one byte per 64x64 source pixel
  ; bit layout for screen: D0=R0 D1=R1 D2=G0 D3=G1 D4=B0 D5=B1
  ; blue = $30, red = $03
start:
  LDX #$00
  LDA #$30
fill_blue:
  STA $0000,X
  STA $0100,X
  STA $0200,X
  STA $0300,X
  STA $0400,X
  STA $0500,X
  STA $0600,X
  STA $0700,X
  STA $0800,X
  STA $0900,X
  STA $0A00,X
  STA $0B00,X
  STA $0C00,X
  STA $0D00,X
  STA $0E00,X
  STA $0F00,X
  INX
  BNE fill_blue
  LDA #$03
  LDX #$20
rect_row_16:
  STA $040F,X
  DEX
  BNE rect_row_16
  LDX #$20
rect_row_17:
  STA $044F,X
  DEX
  BNE rect_row_17
  LDX #$20
rect_row_18:
  STA $048F,X
  DEX
  BNE rect_row_18
  LDX #$20
rect_row_19:
  STA $04CF,X
  DEX
  BNE rect_row_19
  LDX #$20
rect_row_20:
  STA $050F,X
  DEX
  BNE rect_row_20
  LDX #$20
rect_row_21:
  STA $054F,X
  DEX
  BNE rect_row_21
  LDX #$20
rect_row_22:
  STA $058F,X
  DEX
  BNE rect_row_22
  LDX #$20
rect_row_23:
  STA $05CF,X
  DEX
  BNE rect_row_23
  LDX #$20
rect_row_24:
  STA $060F,X
  DEX
  BNE rect_row_24
  LDX #$20
rect_row_25:
  STA $064F,X
  DEX
  BNE rect_row_25
  LDX #$20
rect_row_26:
  STA $068F,X
  DEX
  BNE rect_row_26
  LDX #$20
rect_row_27:
  STA $06CF,X
  DEX
  BNE rect_row_27
  LDX #$20
rect_row_28:
  STA $070F,X
  DEX
  BNE rect_row_28
  LDX #$20
rect_row_29:
  STA $074F,X
  DEX
  BNE rect_row_29
  LDX #$20
rect_row_30:
  STA $078F,X
  DEX
  BNE rect_row_30
  LDX #$20
rect_row_31:
  STA $07CF,X
  DEX
  BNE rect_row_31
done:
  JMP done
