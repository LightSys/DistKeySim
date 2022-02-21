import seaborn as sns
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from plotly import graph_objs as go
import os
from plotly import io as pio


def runDataAnalytics(path, imageFormat):

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
        if imageFormat.upper().startswith("HTM"):
            fig.show()
        else:
            imageFilename = os.path.join(os.path.dirname(path), 'Histogram%d' % (myint)) + "." + imageFormat
            fig.write_image(imageFilename, imageFormat)
            print("  %s image file: %s" %(imageFormat, imageFilename))
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
        if imageFormat.upper().startswith("HTM"):
            plt.show()
        else:
            imageFilename = os.path.join(os.path.dirname(path), 'Density') + "." + imageFormat
            plt.savefig(imageFilename, format=imageFormat)
            print("  %s image file: %s" %(imageFormat, imageFilename))

#Now with a log scale
def runDataAnalyticsLog(path, imageFormat):

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
        if imageFormat.upper().startswith("HTM"):
            fig.show()
        else:
            imageFilename = os.path.join(os.path.dirname(path), 'Histogram%d' % (myint)) + "." + imageFormat
            fig.write_image(imageFilename, imageFormat)
            print("  %s image file: %s" %(imageFormat, imageFilename))
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
        if imageFormat.upper().startswith("HTM"):
            plt.show()
        else:
            imageFilename = os.path.join(os.path.dirname(path), 'Density') + "." + imageFormat
            plt.savefig(imageFilename, format=imageFormat)
            print("  %s image file: %s" %(imageFormat, imageFilename))
