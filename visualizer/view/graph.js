const colors = {
    link: [
        {
            id: -1,
            color: '#cccccc'
        },
        {
            id: 0,
            color: '#3CBAC8',
        },
        {
            id: 1,
            color: '#68ED63',
        },
        {
            id: 2,
            color: '#edea34',
        },
        {
            id: 3,
            color: '#a544ed',
        },
        {
            id: 4,
            color: '#F19181',
        },
    ],
};



const linksToSelectorId = function (selector, src, dst) {
    return selector.replace('#', '') + '-' + src + '-' + dst;
};

const updateEdge = function (rootSelector, srcId, targetId, user) {
    _id = linksToSelectorId(rootSelector, srcId, targetId)

    d3
        .selectAll('#' + _id)
        .style({
            "stroke-width": 5,
            "stroke": colors.link[user + 1].color,
        });
};

const createGraph = function (rootSelector, data, nodeIndexDic) {
    const width = 800;
    const height = 800;
    const node_radius = 10;

    d3.select(rootSelector).html('');

    console.log(nodeIndexDic);

    let x_min = 10000000.0;
    let x_max = -10000000.0;
    let y_min = 10000000.0;
    let y_max = -10000000.0;
    for (let i = 0; i < data.nodes.length; i++) {
        x_min = Math.min(data.nodes[i].x, x_min);
        y_min = Math.min(data.nodes[i].y, y_min);
        x_max = Math.max(data.nodes[i].x, x_max);
        y_max = Math.max(data.nodes[i].y, y_max);
    }
    //console.log(x_max,y_max,x_min, y_min);

    x_1 = (width - 20) / (x_max - x_min);
    x_center = (x_max + x_min) / 2;

    y_1 = (height - 20) / (y_max - y_min);
    y_center = (y_max + y_min) / 2;

    //console.log(x_1, x_center, y_1, y_center);


    const nodes = _.map(data.nodes, function (node, i) {
        return {
            id: node.id,
            index: nodeIndexDic[node.id],
            x: 400 - x_center * x_1 + node.x * x_1,
            y: 400 - y_center * y_1 + node.y * y_1,
            isLambda: data.lambdas.indexOf(node.id) > -1,
        }
    });

    //console.log(nodes);

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
    console.log(JSON.stringify(edges));
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
            });

    // ノード
    const node = svg.selectAll("circle")
        .data(nodes)
        .enter()
        .append("circle")
        .attr({
            r: node_radius,
            opacity: 0.5
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

    // });
    // const sample = d3.select(rootSelector).append("svg")
    //     .attr("class", "sample")
    //     .attr({width: 100, height: height});
    //
    // sample
    //     .selectAll('circle')
    //     .data(colors.link)
    //     .enter()
    //     .append("circle")
    //     .attr({
    //         r: node_radius,
    //         opacity: 0.5
    //     })
    //     .attr("fill", function (d) {
    //         return 'gray';
    //     })
    //     .attr('cx', 50)
    //     .attr('cy', 50)

    // force.start();
};
