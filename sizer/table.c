int table_pc_normal[256]= {
1,2,2,2,2,2,2,2,5,2,2,2,2,6,2,2,
2,2,2,2,2,2,2,2,6,5,6,5,6,5,2,2,
2,2,4,6,5,8,6,2,3,3,6,6,3,4,2,4,
5,5,5,5,5,5,5,5,5,5,2,3,5,6,5,5,
8,6,5,6,6,5,5,7,6,4,5,5,5,6,6,7,
6,7,6,5,6,6,6,6,6,6,5,3,4,3,6,6,
3,6,5,5,5,5,4,5,5,2,3,5,2,6,5,5,
5,5,4,5,4,5,6,6,6,5,5,4,2,4,5,2,
6,2,3,6,5,6,4,4,4,6,5,4,6,2,2,2,
2,2,2,4,4,4,6,7,5,7,5,4,6,6,6,6,
2,2,6,6,6,6,2,6,4,6,4,6,5,4,6,6,
4,6,4,4,3,6,6,3,4,3,5,6,7,7,7,5,
6,6,6,6,6,6,7,6,5,5,5,5,4,4,4,4,
6,6,7,7,7,7,7,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,7,5,5,5,5,5,3,3,4,4,
7,5,5,5,5,5,5,6,6,5,5,5,5,5,6,5
};

int table_oz_normal[256]= {
1,6,6,6,6,6,6,5,5,2,6,6,5,6,6,7,
6,7,6,5,6,5,6,6,6,6,6,6,6,6,6,6,
2,2,4,6,5,8,6,2,3,3,6,6,3,4,2,4,
5,5,5,5,5,5,5,5,5,5,2,3,5,6,5,5,
8,6,5,6,6,5,5,7,6,4,5,5,5,6,6,7,
6,7,6,5,6,6,6,6,6,6,5,3,4,3,6,6,
3,6,5,5,5,5,4,5,5,2,3,5,2,6,5,5,
5,5,4,5,4,5,6,6,6,5,5,4,2,4,5,2,
6,5,5,6,6,6,6,5,5,5,5,4,4,3,6,6,
5,7,7,5,5,5,5,5,5,7,6,6,6,6,6,6,
5,3,5,5,5,6,6,6,5,6,6,7,7,2,6,6,
6,5,7,6,4,6,7,6,5,7,5,7,5,6,6,5,
6,6,6,6,6,6,6,6,6,4,4,3,3,5,4,6,
6,6,6,6,6,6,6,6,6,6,6,6,4,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,5,5,6,6,4,4,3,6,5,4,5,2
};

int table_pc_large[256]= {
1,4,4,4,4,4,4,4,6,4,4,4,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
4,2,4,7,6,10,8,2,4,4,6,6,3,4,2,4,
6,6,6,6,6,6,6,6,6,6,2,3,7,7,7,6,
12,7,7,7,7,6,6,7,7,4,6,7,6,8,7,7,
7,7,7,7,6,7,7,9,7,6,7,4,4,4,6,8,
3,6,6,6,6,6,5,6,6,3,4,6,3,9,6,6,
6,6,5,6,5,6,6,9,6,6,6,4,2,4,7,4,
6,4,3,6,6,6,6,6,4,7,6,4,11,4,4,4,
4,3,3,5,5,4,6,7,5,7,6,4,10,6,6,8,
4,2,6,6,6,6,2,6,4,9,5,6,6,6,9,7,
4,6,5,5,3,6,6,3,4,4,5,6,6,6,6,6,
7,7,7,7,7,7,10,7,6,6,6,6,4,4,4,4,
8,7,7,7,7,7,7,6,7,7,7,7,7,7,7,6,
6,6,6,6,6,6,10,6,6,6,6,6,4,4,4,4,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
};

int table_oz_large[256]= {
1,4,4,4,4,4,4,6,6,6,4,4,6,6,4,4,
4,4,6,6,6,4,4,4,6,6,6,6,6,6,6,6,
4,2,4,7,6,10,8,2,4,4,6,6,3,4,2,4,
6,6,6,6,6,6,6,6,6,6,2,3,7,7,7,6,
12,7,7,7,7,6,6,7,7,4,6,7,6,8,7,7,
7,7,7,7,6,7,7,9,7,6,7,4,4,4,6,8,
3,6,6,6,6,6,5,6,6,3,4,6,3,9,6,6,
6,6,5,6,5,6,6,9,6,6,6,4,2,4,7,4,
4,4,3,6,6,6,6,6,4,7,6,4,11,4,4,4,
4,3,3,5,5,4,6,7,5,7,6,4,10,6,6,8,
4,2,6,6,6,6,2,6,4,6,5,6,6,6,6,7,
4,6,5,5,3,6,6,3,4,4,5,6,6,6,6,6,
6,6,7,7,7,7,10,7,6,6,6,6,4,4,4,4,
8,7,7,7,7,7,7,6,7,7,7,7,7,7,7,6,
6,6,6,6,6,6,10,6,6,6,6,6,4,4,4,4,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6
};
