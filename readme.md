### auto吃掉

- 使用 python 的 opencv 模板匹配并用 win32 api 来模拟键盘敲击。
- 大致原理是第一次首先用 opencv 可以大致确定游戏需要截图的区域，然后在当前大小下截取要识别的头像。

- 循环截屏识别所有并根据位置确定敲击的按键，使用默认 dfjk。

- 使用 sleep 避免过快循环导致的截屏错误，sleep 时间等参数可微调并影响最后成绩。
- 吃掉小鹿乃网址：[吃掉小鹿乃 (xingye.me)](https://xingye.me/game/eatkano/index.php)；吃掉蔡xk网址：[吃掉蔡徐坤 (xxtyme.github.io)](https://xxtyme.github.io/EatCxk/)

更新日期23/1/12 另附c++版
