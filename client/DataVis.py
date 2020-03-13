import pandas as pd
import plotly.graph_objects as go
import plotly.express as px
import numpy as np

def runDataAnalytics(path):
    # Be sure to replace this path with whatever is appropriate
    df = pd.read_csv(path)
    # print(df)

    # First figure produced
    fig = go.Figure()
    for data in df['UUID']:
        d = df['UUID'] == data
        a = df[d]
        if max(a['totalKeys']) == 0:
            sizeVal= [5]* len(a['totalKeys'])
        else:
            sizeVal = (a['totalKeys']/max(a['totalKeys']))*20 + 5
        fig.add_trace(go.Scatter(
            y = a['consumption'],
            x = a['timeSlot'],
            mode='lines+markers',
            connectgaps=True,
            dx=1,
            showlegend=False,
            name=str(data),
            text = a['totalKeys'],
            hoverinfo = 'x+y+text',
            hovertemplate = "Time step: %{x}<br>Consumption: %{y:,.0f}<br>Total keys: %{text}",
            hoveron='points',
            marker=dict(size=sizeVal)
            ))
    fig.update_layout(title="Node consumption and keyspace size over time",xaxis_title="Time", yaxis_title="Consumption (objects created since last time step)")
    fig.update_layout(template="plotly")
    fig.show()

    #Second figure produced
    fig1 = go.Figure()
    for data in df['UUID']:
        d = df['UUID'] == data
        a = df[d]
        if max(a['sharing']) == 0:
            sizeVal= [5]* len(a['sharing'])
        else:
            sizeVal = (a['sharing']/max(a['sharing']))*20 + 5
        fig1.add_trace(go.Scatter(
            y = a['consumption'],
            x = a['timeSlot'],
            mode='lines+markers',
            connectgaps=True,
            dx=1,
            showlegend=False,
            name=str(data),
            text = a['sharing'],
            hoverinfo = 'x+y+text',
            hovertemplate = "Time step: %{x}<br>Consumption: %{y:,.0f}<br>Sharing: %{text}",
            hoveron='points',
            marker=dict(size=sizeVal)
            ))
    fig1.update_layout(title="Node consumption and sharing over time",xaxis_title="Time", yaxis_title="Consumption (objects created since last time step)")
    fig1.update_layout(template="ggplot2")
    fig1.show()

    # Third figure produced
    fig2 = go.Figure()
    for data in df['UUID']:
        d = df['UUID'] == data
        a = df[d]
        fig2.add_trace(go.Scatter(
            y = a['remainder']*100,
            x = a['timeSlot'],
            dy=10,
            mode='lines+markers',
            connectgaps=True,
            dx=1,
            showlegend=False,
            name=str(data),
            hoverinfo = 'x+y',
            hovertemplate = "Time step: %{x}<br>Keyspace Remaining: %{y:.2f}%",
            hoveron='points'
            ))
    fig2.update_layout(title="Local keyspace remaining over time",xaxis_title="Time", yaxis_title="% Total keyspace remaining")
    fig2.update_layout(template="simple_white")
    fig2.show()
