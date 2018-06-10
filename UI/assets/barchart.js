$(function () { 
    var myChart = Highcharts.chart('container', {
        chart: {
            type: 'bar'
        },
        title: {
            text: 'Chemo Cycle Effectiveness Analysis'
        },
        xAxis: {
            title: {
                text: 'Treatments'
            },
            categories: ['1-5', '6', '>6']
        },
        yAxis: {
            title: {
                text: 'Survival'
            }
        },
        series: [{
            name: 'Number Patients',
            data: [199, 500, 300]
        }]
    });
});