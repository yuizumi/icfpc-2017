const Visualizer = function (rootSelector) {
    /*
     描画するrootのセレクタを指定してnewで呼び出してください
     */

    this.init = function () {
        this.RootSelector = rootSelector;
        this.MapData = {};
        this.NodeIndexDic = {};
    };

    this.setMapData = function (mapData) {
        this.MapData = JSON.stringify(mapData);
    };

    this.getMapData = function () {
        return JSON.parse(this.MapData);
    };

    // マップの初期化
    this.createMap = function (data) {
        /*
         data: {
         nodes: [
            {
                id: 1,
                x: 1,
                y: 1
            },
            
         ]
         edged: [[1, 2], [1, 5], [2, 3] ...],
         lambdas: [1 ...]
         }
         */

        this.NodeIndexDic = {};
        for (let i = 0; i < data.nodes.length; i++) {
            this.NodeIndexDic[data.nodes[i].id] = i;
        }

        this.setMapData(data);
        createGraph(
            this.RootSelector,
            this.getMapData(),
            this.NodeIndexDic
        );
    };


    // エッジの更新
    this.update = function (src_node_id, dst_node_id, user_id) {
        console.log('user' + user_id + ' claimed ' + src_node_id + '->' + dst_node_id);
        updateEdge(this.RootSelector, src_node_id, dst_node_id, user_id);
    };


    //オブジェクトのloadを終えたらinitする
    this.init();
};

// テスト
const VisualizeTest = function () {
    let vis = new Visualizer('#main-visualize-cell');

    vis.createMap({
        nodes: [
            {
                id: 0,
                x: 20,
                y: 50
            },
            {
                id: 1,
                x: 50,
                y: 50
            },
            {
                id: 2,
                x: 50,
                y: 100
            },
            {
                id: 5,
                x: 100,
                y: 100
            }
        ],
        edges: [
            // [0, 5],
            [0, 1],
            [0, 2]
        ],
        lambdas: [1]
    });

    vis.update(0, 1, 1);
    return vis;
};

VisualizeTest();