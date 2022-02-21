import numpy as np
import os
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
import sys

def runDataAnalytics(path, imageFormat):
    # Be sure to replace this path with whatever is appropriate
    df = pd.read_csv(path)
    lenData = len(df)

    print("First Figure: Node consumption and keyspace over time")
    fig = go.Figure()
    i = 0
    for data in df['UUID']:
        percentComplete = i * 100 / lenData
        if i % 100 == 0:
            print('{:d}%'.format(int(percentComplete)), end='\r', flush=True)
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
        i = i + 1
    fig.update_layout(title="Node consumption and keyspace size over time",xaxis_title="Time", yaxis_title="Consumption (objects created since last time step)")
    fig.update_layout(template="plotly")
    if imageFormat.upper().startswith("HTM"):
        fig.show()
    else:
        imageFilename = os.path.join(os.path.dirname(path), 'NodeConsumptionAndKeyspace') + "." + imageFormat
        fig.write_image(imageFilename, imageFormat)
        print("  %s image file: %s" %(imageFormat, imageFilename))

    print("Second Figure: Node consumption and sharing over time")
    fig1 = go.Figure()
    i = 0
    for data in df['UUID']:
        if i % 100 == 0:
            print('{:d}%'.format(int(percentComplete)), end='\r', flush=True)
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
        i = i + 1
    fig1.update_layout(title="Node consumption and sharing over time",xaxis_title="Time", yaxis_title="Consumption (objects created since last time step)")
    fig1.update_layout(template="ggplot2")
    if imageFormat.upper().startswith("HTM"):
        fig1.show()
    else:
        imageFilename = os.path.join(os.path.dirname(path), 'NodeConsumptionAndSharing') + "." + imageFormat
        fig1.write_image(imageFilename, imageFormat)
        print("  %s image file: %s" %(imageFormat, imageFilename))

    print("Third Figure: Local keyspace remaining over time")
    fig2 = go.Figure()
    i = 0
    for data in df['UUID']:
        if i % 100 == 0:
            print('{:d}%'.format(int(percentComplete)), end='\r', flush=True)
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
        i = i + 1
    fig2.update_layout(title="Local keyspace remaining over time",xaxis_title="Time", yaxis_title="% Total keyspace remaining")
    fig2.update_layout(template="simple_white")
    if imageFormat.upper().startswith("HTM"):
        fig2.show()
    else:
        imageFilename = os.path.join(os.path.dirname(path), 'LocalKeyspace') + "." + imageFormat
        fig2.write_image(imageFilename, imageFormat)
        print("  %s image file: %s" %(imageFormat, imageFilename))
