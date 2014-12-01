# LDA

LDA の変分ベイズの実装  
詳細は気が向いたら追加  

## 入力

以下のような形式

    1 1 2 3 1 3
    2 3 4 1 2
    5 3 1 2 3 3

一行が一文書に対応  
数字は単語 ID  

## 使い方

    ./ilda -I brown.txt.train -T brown.txt.test -o model -k 100 -i 100 -r 1e-5

例えば上のだと brown.txt.train というファイルからトピック数 k = 100 の LDA を学習  
反復回数は 100 回、収束半径は 1e-5 に設定  
学習された LDA を用いて brown.txt.test に対して VFE と Perplexity を計算  
学習後のハイパーパラメータは model.{al, bt} に出力  
学習後の変分パラメータは model.{alp, btp, thp, php} に出力  
出力は以下

    #### Setting ####
    Model     : model
    Train     : brown.txt.train
    test      : brown.txt.test
    seed      : 1417409686
    # iter    : 100
    radius    : 1.000000e-05
    # docs    : 500
    # topics  : 10
    # words   : 2177

    #### Training ####
        0 -5.050484334624503e+06 0.000000e+00 3.91932e-02
        1 -2.037908976656124e+06 1.478268e+00 9.26611e-02
        ...
        97 -1.930475362347997e+06 1.573693e-05 5.36131e+00
        98 -1.930446071356246e+06 1.517317e-05 5.41583e+00
        99 -1.930418339333334e+06 1.436581e-05 5.46730e+00
        100 -1.930392593240308e+06 1.333723e-05 5.52171e+00

    #### Results ####
    -1.930392593240308e+06
    -1.888741603978806e+06
    -3.557132129863850e+05

出力の最後の3行はそれぞれ以下である  
学習後の変分自由エネルギー  
学習後の training set に対する Perplexity  
学習後の test set に対する Perplexity  
