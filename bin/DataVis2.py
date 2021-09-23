import seaborn as sns
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from plotly import graph_objs as go
import os
from plotly import io as pio


def runDataAnalytics(path):

    # Be sure to replace this path with whatever is appropriate
    df = pd.read_csv(path)
    myint = 0
    # print(df)
    # Produces all histograms
	
    for data in df['UUID'].unique():
        dd = df['UUID'] == data
        aa = df[dd]
		
        fig = go.Figure()
        fig.add_trace(go.Histogram(x=aa['totalKeys']))
        fig.add_trace(go.Histogram(x=aa['consumption']))
        fig.add_trace(go.Histogram(x=aa['sharing']))

        # Overlay both histograms and label with a title
        data = df['UUID'][myint]
        titleStr = 'Histogram for Node with UUID ' + str(data)
        fig.update_layout(barmode='overlay', title = titleStr)
        # Reduce opacity to see both histograms
        fig.update_traces(opacity=0.75)
        fig.show()
        myint = myint + 1

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
        print("here? At last?")
#Now with a log scale

def runDataAnalyticsLog(path):

    # Be sure to replace this path with whatever is appropriate
    df = pd.read_csv(path)
    myint = 0
    # print(df)
    # Produces all histograms
	
    for data in df['UUID'].unique():
        dd = df['UUID'] == data
        aa = df[dd]
		
        fig = go.Figure()
        fig.add_trace(go.Histogram(x=aa['totalKeys']))
        fig.add_trace(go.Histogram(x=aa['consumption']))
        fig.add_trace(go.Histogram(x=aa['sharing']))

        # Overlay both histograms and label with a title
        data = df['UUID'][myint]
        titleStr = 'Histogram for Node with UUID ' + str(data)
        fig.update_layout(barmode='overlay', title = titleStr)
        # Reduce opacity to see both histograms
        fig.update_traces(opacity=0.75)
        fig.update_yaxes(type="log")
        fig.show()
        myint = myint + 1

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
        plt.yscale("log")
        plt.show()
        print("here? At last?")
