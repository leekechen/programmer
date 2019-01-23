h = wr_open()
wr_set(h, "writer", "writerCRC32:0xEA76A676")
wr_updatefirm(h, "772")
wr_close()
