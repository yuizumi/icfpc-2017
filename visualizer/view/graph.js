const INF = 10000000.0;
const scoreSelector = '#score-cell';
const colors = {
    link: [
        {
            id: -1,
            color: '#bfbfbf',
        },
        {
            id: 0,
            color: '#F19181',
        },
        {
            id: 1,
            color: '#33a3c8',
        },
        {
            id: 2,
            color: '#0ba72d',
        },
        {
            id: 3,
            color: '#edea34',
        },
        {
            id: 4,
            color: '#8b2fed',
        },
    ],
};

const drawLabels = function () {
    d3.select(scoreSelector).html('');
    const rect = d3.select(scoreSelector)
        .selectAll("div")
        .data(colors.link)
        .enter()
        .append("div")
        .style("background-color", function (d) {
            return d.color
        })
        .style(
            {
                "display": "inline-block",
                "padding": "2px 10px",
                "margin": "2px 4px",
                "font-size": "18px",
                "width": "180px"
            }
        )
        .html(function (d) {
            if (d.id < 0) {
                return "<p style='margin:0'>empty</p><span></span>"
            }
            return "<p style='margin:0'> user" + d.id + " : </p>"
                + "<span id=" + scoreSelectorId(d.id) + ">0</span>"
        })
        .selectAll('span')
        .style({
            "font-weight": "bold",
            "text-align": "right",
            "display": "inline-block",
            "width": "100%",
        })

};

const drawScore = function (scores) {
    for (let i = 0; i < scores.length; i++) {
        userId = scores[i]["punter"];
        score = scores[i]["score"];
        const _id = scoreSelectorId(userId);
        d3.selectAll('#' + _id)
            .text(score.toLocaleString())
    }
};

const scoreSelectorId = function (user) {
    return "score-" + user;
};

const linksToSelectorId = function (selector, src, dst) {
    return selector.replace('#', '') + '-' + src + '-' + dst;
};

const updateEdge = function (rootSelector, srcId, targetId, user) {
    const _id = linksToSelectorId(rootSelector, srcId, targetId)

    const strokeWidth = user < 0 ? 2 : 4;
    const dash = user < 0 ? "5, 5" : "5, 0";
    d3
        .selectAll('#' + _id)
        .style({
            "stroke-width": strokeWidth,
            "stroke": colors.link[user + 1].color,
        })
        .attr("stroke-dasharray", dash);

};


const createGraph = function (rootSelector, data, nodeIndexDic) {
    const width = 900;
    const height = 900;
    const nodeRadius = 12;
    const svgMargin = 24;

    d3.select(rootSelector).html('');

    let xMin = INF;
    let xMax = -INF;
    let yMin = INF;
    let yMax = -INF;
    for (let i = 0; i < data.nodes.length; i++) {
        xMin = Math.min(data.nodes[i].x, xMin);
        yMin = Math.min(data.nodes[i].y, yMin);
        xMax = Math.max(data.nodes[i].x, xMax);
        yMax = Math.max(data.nodes[i].y, yMax);
    }
    //console.log(xMax,yMax,xMin, yMin);

    x_1 = (width - svgMargin) / (xMax - xMin);
    x_center = (xMax + xMin) / 2;

    y_1 = (height - svgMargin) / (yMax - yMin);
    y_center = (yMax + yMin) / 2;

    //console.log(x_1, x_center, y_1, y_center);
    // ノードデータをvisualize用に変換
    const nodes = _.map(data.nodes, function (node, i) {
        return {
            id: node.id,
            index: nodeIndexDic[node.id],
            x: (width / 2) - x_center * x_1 + node.x * x_1,
            y: (height / 2) - y_center * y_1 + node.y * y_1,
            isLambda: data.lambdas.indexOf(node.id) > -1,
        }
    });

    const edges = _.map(data.edges, function (edge, i) {
        return {
            source: nodes[nodeIndexDic[edge[0]]],
            target: nodes[nodeIndexDic[edge[1]]],
            sourceId: edge[0],
            targetId: edge[1],
            user: -1
        }
    });

    const svg = d3.select(rootSelector).append("svg")
        .attr({width: width, height: height});

    //リンク
    const link = svg.selectAll("line")
        .data(edges)
        .enter()
        .append("line")
        .style({
            "stroke-width": 2
        })
        .attr("stroke", function (edge) {
            return colors.link[edge.user + 1].color;
        })
        .attr("id", function (edge) {
            return linksToSelectorId(
                rootSelector, edge.sourceId, edge.targetId
            );
        })
        .attr(
            {
                x1: function (d) {
                    return d.source.x;
                },
                y1: function (d) {
                    return d.source.y;
                },
                x2: function (d) {
                    return d.target.x;
                },
                y2: function (d) {
                    return d.target.y;
                }
            })
        .attr("stroke-dasharray", "5, 5");

    // ノード
    const node = svg.selectAll("circle")
        .data(nodes)
        .enter()
        .append("circle")
        .attr({
            r: nodeRadius,
            opacity: 0.8
        })
        .attr("fill", function (d) {
            return d.isLambda ? "red" : "gray";
        })
        .attr({
            cx: function (d) {
                return d.x;
            },
            cy: function (d) {
                return d.y;
            }
        });

    const label = svg.selectAll('text')
        .data(nodes)
        .enter()
        .append('text')
        .attr({
            "text-anchor": "middle",
            "fill": "white"
        })
        .style({"font-size": 18, "font-weight": "bold"})
        .text(function (d) {
            return d.id;
        })
        .attr({
            x: function (d) {
                return d.x;
            },
            y: function (d) {
                return d.y + 5;
            }
        });
    // force.start();
};
 