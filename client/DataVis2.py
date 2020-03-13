import seaborn as sns
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import os
import plotly.io as pio


def runDataAnalytics(path):

    # Be sure to replace this path with whatever is appropriate
    df = pd.read_csv(path)
    # print(df)

    # Produces all histogams
    for data in df['UUID'].unique():
        dd = df['UUID'] == data
        aa = df[dd]

        fig = go.Figure()
        fig.add_trace(go.Histogram(x=aa['totalKeys']))
        fig.add_trace(go.Histogram(x=aa['consumption']))
        fig.add_trace(go.Histogram(x=aa['sharing']))

        # Overlay both histograms and label with a title
        data = df['UUID'][0]
        titleStr = 'Histogram for Node with UUID ' + str(data)
        fig.update_layout(barmode='overlay', title = titleStr)
        # Reduce opacity to see both histograms
        fig.update_traces(opacity=0.75)
        fig.show()

    # Produces all histogams
    for data in df['UUID'].unique():
        dd = df['UUID'] == data
        aa = df[dd]
        # plot of 2 variables
        p1=sns.kdeplot(aa['totalKeys'], shade=True, color="b")
        p1=sns.kdeplot(aa['consumption'], shade=True, color="r")
        p1=sns.kdeplot(aa['sharing'], shade=True, color="g")
        titleStr = 'Density distribution for Node with UUID ' + str(data)
        p1.set_title(titleStr)
        plt.xlim(0, max(max(aa['totalKeys']), max(aa['consumption'])))

        plt.show()
