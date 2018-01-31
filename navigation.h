#ifndef NAVIGATION_H
#define NAVIGATION_H

struct MyGraph{                                         //图的定义
    int vexnum;                                         //顶点数
    int arcs[MAX_VERTEXNUM][MAX_VERTEXNUM];             //邻接矩阵
};

class place {                                           //地点的定义
public:
    char name[10];
    int time;
    place() {
        name[0] = '\0';
        time = 0;
    }
    void init(char* na, int t) {
        strcpy(name, na);
        time = t;
    }
};

char Read(FILE* fp) {//读入数据
    int num = 0;
    char ch;
    while (1) {
        ch = fgetc(fp);
        if (ch >= '0'&&ch <= '9') {
            num = num * 10 + (int)ch - 48;
        }
        else if (ch == '-') {
            ch = fgetc(fp);
            ch = fgetc(fp);
            return -1;
        }
        else
            return num;
    }
}

class MapAlgorithm{
public:
    MapAlgorithm();
    void CreateGraph();
    QVector<int> getpath_single(int endPos);     //获取单源最短路的路线
    QVector<int> getpath_loop();                 //获取最小哈密顿回路的路线
};

#endif // NAVIGATION_H
