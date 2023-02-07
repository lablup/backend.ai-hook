import numpy as np
import os
import pandas as pd


def test_cpu_count():
    assert os.sched_getaffinity(0) == {0, 1, 4, 5}
    assert os.cpu_count() == 4


def test_fbprophet():
    from mpl_toolkits.mplot3d import Axes3D
    from sklearn.preprocessing import StandardScaler
    import matplotlib
    import matplotlib.pyplot as plt # plotting

    matplotlib.use('agg')
    common_path = '/tmp/'
    os.makedirs('/tmp/output', exist_ok=True)

    def plotHistogram(df, nHistogramShown, nHistogramPerRow):
        print("plotHistogram: step 0")
        nunique = df.nunique()
        df = df[[col for col in df if nunique[col] > 1 and nunique[col] < 50]]
        nRow, nCol = df.shape
        columnNames = list(df)
        nHistRow = (nCol + nHistogramPerRow - 1) // nHistogramPerRow
        print("plotHistogram: step 1 (create figure)")
        plt.figure(num=None, figsize=(6*nHistogramPerRow, 9*nHistRow), dpi=80, facecolor='w', edgecolor='k')
        print("plotHistogram: step 2 (plot)")
        for i in range(min(nCol, nHistogramShown)):
            plt.subplot(nHistRow, nHistogramPerRow, i+1)
            print("plotHistogram: step 2 (subplot {})".format(i))
            df.iloc[:,i].hist()
            plt.ylabel('counts', fontsize=20)
            plt.xticks(rotation=90)
            plt.title(f'{columnNames[i]} (column {i})', fontsize=20)
        print("plotHistogram: step 3 (layout)")
        plt.tight_layout(pad=1.0, w_pad=1.0, h_pad=1.0)
        plt.show()
        print("plotHistogram: step 4 (savefig)")
        plt.savefig(common_path + 'output/plot_histogram.png')
        print("plotHistogram: finished")

    def plotCorrelationMatrix(df, graphWidth):
        print("plotCorrelationMatrix: step 0")
        filename = df.dataframeName
        df = df.dropna('columns')
        df = df[[col for col in df if df[col].nunique() > 1]]
        if df.shape[1] < 2:
            print(f'No correlation plots shown: The number of non-NaN or constant columns ({df.shape[1]}) is less than 2')
            return
        print("plotCorrelationMatrix: step 1")
        corr = df.corr()
        print("plotCorrelationMatrix: step 2 (create figure)")
        plt.figure(num=None, figsize=(graphWidth, graphWidth), dpi=80, facecolor='w', edgecolor='k')
        print("plotCorrelationMatrix: step 3 (plot)")
        corrMat = plt.matshow(corr, fignum = 1)
        plt.xticks(range(len(corr.columns)), corr.columns, rotation=90)
        plt.yticks(range(len(corr.columns)), corr.columns)
        plt.gca().xaxis.tick_bottom()
        plt.colorbar(corrMat)
        plt.title(f'Correlation Matrix for {filename}', fontsize=15)
        plt.show()
        print("plotCorrelationMatrix: step 4 (savefig)")
        plt.savefig(common_path+'output/plot_correlation_matrix.png')
        print("plotCorrelationMatrix: finished")

    df1 = pd.read_csv('assets/pizzeria_tania_data_simplified.csv', delimiter=',')
    df1.dataframeName = 'pizzeria_tania_data_simplified.csv'
    nRow, nCol = df1.shape
    print(f'There are {nRow} rows and {nCol} columns')

    plotHistogram(df1, 10, 5)
    plotCorrelationMatrix(df1, 8)
    figfiles = os.listdir(common_path + 'output')
    assert len([file for file in figfiles if file.startswith("plot")]) == 2


def run_tests():
    print('TEST: cpu_count')
    test_cpu_count()
    print('TEST: fbprophet')
    test_fbprophet()
    print('Passed all tests!')


if __name__ == '__main__':
    run_tests()
