// at45x statusstatic unsigned at45_status(){	char req[2];	req[0] = 0xd7;	at45_transfer(req, sizeof(req), 0, 0);	return req[1];}// at45x waiting readystatic void at45_wait_rdy() {	while (!(at45_status() & 0x80));}