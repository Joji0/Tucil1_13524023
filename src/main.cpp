#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#define PCLOSE_CMD _pclose
#define POPEN_FUNC _popen
#else
#define PCLOSE_CMD pclose
#define POPEN_FUNC popen
#endif
using namespace std;
using namespace sf;

const unsigned int W_WIDTH = 1100;
const unsigned int W_HEIGHT = 750;
const Color C_BG(30, 30, 30);
const Color C_PANEL(44, 62, 80);
const Color C_BTN(52, 73, 94);
const Color C_BTN_H(93, 109, 126);
const Color C_BTN_A(46, 204, 113);
const Color C_BTN_R(231, 76, 60);
const Color C_BTN_B(52, 152, 219);
const Color C_TXT(236, 240, 241);
const Color C_SUB(189, 195, 199);

int n = 0, m = 0;
vector<string> grid;
vector<int> ans;
int64_t iter = 0;
bool found = false, solving = false, stop = false;
int delay_ms = 0, mode = 2;
double elapsed_time = 0.0;
string file_path = "";
string save_msg = "";
Texture q_tex;
bool has_tex = false;

string get_file_dialog(int mode) {
  string cmd;
  string res = "";
  char buf[128];
#ifdef _WIN32
  cmd = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe -command \"Add-Type -AssemblyName "
        "System.Windows.Forms; $f = New-Object System.Windows.Forms.";
  cmd += (mode == 0 ? "OpenFileDialog" : "SaveFileDialog");
  cmd += "; $f.Filter = '";
  if (mode == 0)
    cmd += "Text Files (*.txt)|*.txt|All Files (*.*)|*.*";
  else if (mode == 1)
    cmd += "PNG Image (*.png)|*.png";
  else
    cmd += "Text Files (*.txt)|*.txt";
  cmd += "'; $f.ShowDialog() | Out-Null; $f.FileName\"";
#elif __APPLE__
  if (mode == 0) {
    cmd = "osascript -e 'POSIX path of (choose file of type {\"txt\"} with prompt \"Select Board\")'";
  } else if (mode == 1) {
    cmd = "osascript -e 'POSIX path of (choose file name with prompt \"Save Solution\" default name "
          "\"solution.png\")'";
  } else {
    cmd = "osascript -e 'POSIX path of (choose file name with prompt \"Save Solution\" default name "
          "\"solution.txt\")'";
  }
#else
  cmd = "zenity --file-selection";
  if (mode == 0)
    cmd += " --file-filter=*.txt";
  else if (mode == 1)
    cmd += " --save --filename=solution.png --confirm-overwrite";
  else
    cmd += " --save --filename=solution.txt --confirm-overwrite";
#endif
  FILE *pipe = POPEN_FUNC(cmd.c_str(), "r");
  if (!pipe) return "";
  while (fgets(buf, sizeof buf, pipe) != NULL) {
    res += buf;
  }
  PCLOSE_CMD(pipe);
  res.erase(remove(res.begin(), res.end(), '\n'), res.end());
  res.erase(remove(res.begin(), res.end(), '\r'), res.end());
  return res;
}
Color get_col(char c) {
  if (c == ' ' || c == 0) return Color(20, 20, 20);
  int v = (c >= 'a') ? (c - 'a') : (c - 'A');
  return Color((v * 60 + 20) % 255, (v * 90 + 40) % 255, (v * 40 + 100) % 255);
}
void sync_vis(const vector<int> &curr) {
  if (delay_ms > 0) {
    ans = curr;
    this_thread::sleep_for(chrono::microseconds(delay_ms));
  }
}
string validate_board() {
  if (n == 0) return "Board is empty";
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++) {
      char c = grid[i][j];
      if (c < 'A' || c > 'Z') return "Invalid char '" + string(1, c) + "'\nat row " + to_string(i + 1);
    }
  vector<bool> seen(26, false);
  int cnt = 0;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++) {
      int idx = grid[i][j] - 'A';
      if (!seen[idx]) {
        seen[idx] = true;
        cnt++;
      }
    }
  if (cnt != n) return "Need " + to_string(n) + " regions,\nfound " + to_string(cnt);
  return "";
}
void solve_bf() {
  vector<int> curr(n, 0);
  while (!stop) {
    iter++;
    sync_vis(curr);
    if (delay_ms == 0 && iter % 5000 == 0) ans = curr;
    bool ok = true;
    for (int i = 0; i < n; i++) {
      int c = curr[i];
      if (c >= m) {
        ok = false;
        break;
      }
      char col = grid[i][c];
      for (int prev = 0; prev < i; prev++) {
        int pc = curr[prev];
        if (pc == c) {
          ok = false;
          break;
        }
        if (grid[prev][pc] == col) {
          ok = false;
          break;
        }
      }
      if (!ok) break;
      if (i > 0) {
        if (abs(c - curr[i - 1]) <= 1) {
          ok = false;
          break;
        }
      }
    }
    if (ok) {
      ans = curr;
      found = true;
      return;
    }
    int idx = n - 1;
    while (idx >= 0) {
      curr[idx]++;
      if (curr[idx] < m) break;
      curr[idx] = 0;
      idx--;
    }
    if (idx < 0) return;
  }
}
bool safe(int r, int c, const vector<int> &curr) {
  if (c >= m) return false;
  char col = grid[r][c];
  for (int pr = 0; pr < r; pr++) {
    int pc = curr[pr];
    if (pc == c) return false;
    if (grid[pr][pc] == col) return false;
    if (pr == r - 1 && abs(pc - c) <= 1) return false;
  }
  return true;
}
void solve_bt(int r, vector<int> &curr) {
  if (found || stop) return;
  if (r == n) {
    ans = curr;
    found = true;
    return;
  }
  for (int c = 0; c < m; c++) {
    iter++;
    curr[r] = c;
    sync_vis(curr);
    if (delay_ms == 0 && iter % 2000 == 0) ans = curr;
    if (safe(r, c, curr)) {
      solve_bt(r + 1, curr);
      if (found) return;
    }
    curr[r] = -1;
  }
}
void run() {
  auto start = chrono::high_resolution_clock::now();
  vector<int> tmp(n, (mode == 1 ? 0 : -1));
  ans = tmp;
  if (mode == 1)
    solve_bf();
  else
    solve_bt(0, tmp);
  auto end = chrono::high_resolution_clock::now();
  elapsed_time = chrono::duration<double, milli>(end - start).count();
  solving = false;
}
void draw(RenderTarget &t, float w, float h, float sx, float sy) {
  if (n == 0 || m == 0) return;
  float ratio_g = (float)m / n;
  float ratio_a = w / h;
  float sz, fw, fh;
  if (ratio_g > ratio_a) {
    fw = w;
    sz = w / m;
    fh = sz * n;
  } else {
    fh = h;
    sz = h / n;
    fw = sz * m;
  }
  float ox = sx + (w - fw) / 2.0f;
  float oy = sy + (h - fh) / 2.0f;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      RectangleShape cell(Vector2f(sz - 2.f, sz - 2.f));
      cell.setPosition({ox + j * sz + 1.f, oy + i * sz + 1.f});
      cell.setFillColor(get_col(grid[i][j]));
      t.draw(cell);
      if (i < (int)ans.size() && ans[i] == j) {
        if (has_tex) {
          Sprite s(q_tex);
          Vector2u ts = q_tex.getSize();
          float sc = (sz * 0.8f) / max(ts.x, ts.y);
          s.setScale({sc, sc});
          s.setOrigin({ts.x / 2.f, ts.y / 2.f});
          s.setPosition({ox + j * sz + sz / 2.f, oy + i * sz + sz / 2.f});
          t.draw(s);
        } else {
          CircleShape c(sz * 0.35f);
          c.setFillColor(Color::Black);
          c.setOutlineThickness(2.f);
          c.setOutlineColor(Color::White);
          c.setOrigin({sz * 0.35f, sz * 0.35f});
          c.setPosition({ox + j * sz + sz / 2.f, oy + i * sz + sz / 2.f});
          t.draw(c);
        }
      }
    }
  }
}
void save_txt() {
  if (!found || n == 0) return;
  string path = get_file_dialog(2);
  if (path.empty() || path == "ERROR") return;
  if (path.length() < 4 || path.substr(path.length() - 4) != ".txt") {
    path += ".txt";
  }
  ofstream out(path);
  if (!out) {
    save_msg = "Error saving file!";
    return;
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      if (ans[i] == j)
        out << '#';
      else
        out << grid[i][j];
    }
    out << '\n';
  }
  out << "\nWaktu pencarian: " << (int64_t)elapsed_time << " ms\n";
  out << "Banyak kasus yang ditinjau: " << iter << " kasus\n";
  out.close();
  save_msg = "Saved: " + path;
  cout << save_msg << endl;
}
void save_img() {
  if (!found || n == 0) return;
  string path = get_file_dialog(1);
  if (path.empty() || path == "ERROR") return;
  unsigned int iw = 1200;
  unsigned int ih = (unsigned int)(1200.0f * ((float)n / m));
  if (ih < 600) ih = 600;
  RenderTexture rt;
  if (!rt.resize({iw, ih})) return;
  rt.clear(C_BG);
  draw(rt, (float)iw, (float)ih, 0.f, 0.f);
  rt.display();
  if (rt.getTexture().copyToImage().saveToFile(path)) {
    save_msg = "Saved: " + path;
    cout << save_msg << endl;
  } else {
    save_msg = "Error saving image!";
  }
}
struct Btn {
  RectangleShape s;
  Text t;
  bool h = false, a = false;
  Btn(Vector2f sz, Vector2f p, string l, Font &f, Color c = C_BTN) : t(f) {
    s.setSize(sz);
    s.setPosition(p);
    s.setFillColor(c);
    t.setString(l);
    t.setCharacterSize(14);
    t.setFillColor(C_TXT);
    FloatRect b = t.getLocalBounds();
    t.setPosition({floor(p.x + (sz.x - b.size.x) / 2.f), floor(p.y + (sz.y - b.size.y) / 2.f - 4.f)});
  }
  bool upd(Vector2f mp, bool clk) {
    h = s.getGlobalBounds().contains(mp);
    if (a)
      s.setFillColor(C_BTN_A);
    else if (h)
      s.setFillColor(C_BTN_H);
    else if (s.getFillColor() == C_BTN_H || s.getFillColor() == C_BTN_A) {
      if (!a) s.setFillColor(C_BTN);
    }
    return h && clk;
  }
  void drw(RenderWindow &w) {
    w.draw(s);
    w.draw(t);
  }
};
struct Inp {
  RectangleShape s;
  Text t;
  string val;
  Inp(Vector2f sz, Vector2f p, Font &f) : t(f) {
    s.setSize(sz);
    s.setPosition(p);
    s.setFillColor(Color(50, 50, 50));
    s.setOutlineColor(Color::White);
    s.setOutlineThickness(1.f);
    t.setCharacterSize(14);
    t.setFillColor(Color::Yellow);
    t.setPosition({p.x + 5.f, p.y + 7.f});
    t.setString("Select file...");
  }
  void set(string v) {
    val = v;
    size_t sl = v.find_last_of("/\\");
    string d = (sl != string::npos) ? v.substr(sl + 1) : v;
    if (d.length() > 25) d = "..." + d.substr(d.length() - 22);
    t.setString(d);
    t.setFillColor(Color::White);
  }
  void drw(RenderWindow &w) {
    w.draw(s);
    w.draw(t);
  }
};
struct Sld {
  RectangleShape tr, hd;
  Text lbl;
  float mn, mx, cur;
  bool drag = false;
  Sld(Vector2f p, float w, float minv, float maxv, Font &f) : lbl(f) {
    mn = minv;
    mx = maxv;
    cur = minv;
    tr.setSize({w, 6.f});
    tr.setPosition(p);
    tr.setFillColor(Color(149, 165, 166));
    hd.setSize({16.f, 20.f});
    hd.setOrigin({8.f, 10.f});
    hd.setFillColor(Color::White);
    recalc();
    lbl.setCharacterSize(13);
    lbl.setFillColor(C_TXT);
    lbl.setPosition({p.x, p.y - 22.f});
    lbl.setString("Speed: TURBO");
  }
  void recalc() {
    float pct = (cur - mn) / (mx - mn);
    hd.setPosition({tr.getPosition().x + pct * tr.getSize().x, tr.getPosition().y + tr.getSize().y / 2.f});
  }
  void upd(RenderWindow &w) {
    Vector2f mp = w.mapPixelToCoords(Mouse::getPosition(w));
    if (Mouse::isButtonPressed(Mouse::Button::Left)) {
      FloatRect hb = tr.getGlobalBounds();
      hb.position.y -= 10.f;
      hb.size.y += 20.f;
      if (hd.getGlobalBounds().contains(mp) || hb.contains(mp)) drag = true;
    } else
      drag = false;
    if (drag) {
      float mx_pos = max(tr.getPosition().x, min(mp.x, tr.getPosition().x + tr.getSize().x));
      float pct = (mx_pos - tr.getPosition().x) / tr.getSize().x;
      cur = mn + pct * (mx - mn);
      recalc();
      if (cur <= 50) {
        lbl.setString("Speed: TURBO");
        lbl.setFillColor(Color::Cyan);
      } else if (cur < 5000) {
        lbl.setString("Speed: FAST");
        lbl.setFillColor(Color::Green);
      } else {
        lbl.setString("Speed: SLOW");
        lbl.setFillColor(Color(255, 100, 100));
      }
    }
  }
  void drw(RenderWindow &w) {
    w.draw(tr);
    w.draw(hd);
    w.draw(lbl);
  }
};
int main() {
  RenderWindow w(VideoMode(Vector2u(W_WIDTH, W_HEIGHT)), "Queens Solver");
  w.setFramerateLimit(60);
  Font f;
  if (!f.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") &&
      !f.openFromFile("C:/Windows/Fonts/arial.ttf") &&
      !f.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf") && !f.openFromFile("font.ttf")) {
  }
  vector<string> p_tex = {"queen.png", "src/queen.png"};
  for (auto &p : p_tex)
    if (q_tex.loadFromFile(p)) {
      has_tex = true;
      q_tex.setSmooth(true);
      break;
    }
  RectangleShape pn(Vector2f(300.f, (float)W_HEIGHT));
  pn.setFillColor(C_PANEL);
  Text tit(f, "QUEENS SOLVER", 24);
  tit.setPosition({20.f, 20.f});
  tit.setStyle(Text::Bold);
  Text nm(f, "Made by Jonathan Kris Wicaksono - 13524023", 12);
  nm.setPosition({20.f, 50.f});
  nm.setFillColor(C_SUB);
  Text l_f(f, "File:", 14);
  l_f.setPosition({20.f, 90.f});
  Inp inp({260.f, 30.f}, {20.f, 115.f}, f);
  Btn b_brs({260.f, 30.f}, {20.f, 155.f}, "BROWSE", f, C_BTN_B);
  Btn b_ld({260.f, 35.f}, {20.f, 195.f}, "LOAD", f);
  Text l_m(f, "Algo:", 14);
  l_m.setPosition({20.f, 250.f});
  Btn b_m1({260.f, 30.f}, {20.f, 275.f}, "Brute Force", f);
  Btn b_m2({260.f, 30.f}, {20.f, 315.f}, "Backtrack", f);
  b_m2.a = true;
  Sld sld({20.f, 390.f}, 260.f, 0.f, 300000.f, f);
  Btn b_run({260.f, 50.f}, {20.f, 440.f}, "SOLVE", f, C_BTN_A);
  Btn b_stp({260.f, 40.f}, {20.f, 500.f}, "STOP", f, C_BTN_R);
  Btn b_svt({260.f, 35.f}, {20.f, 620.f}, "SAVE TXT", f, C_BTN_A);
  Btn b_sv({260.f, 35.f}, {20.f, 660.f}, "SAVE IMG", f, C_BTN_B);
  Text st(f, "Idle", 14);
  st.setPosition({20.f, 560.f});
  st.setLineSpacing(1.2f);
  while (w.isOpen()) {
    while (const optional ev = w.pollEvent()) {
      if (ev->is<Event::Closed>()) {
        stop = true;
        if (solving) this_thread::sleep_for(chrono::milliseconds(200));
        w.close();
      }
      if (const auto *mb = ev->getIf<Event::MouseButtonPressed>()) {
        if (mb->button == Mouse::Button::Left) {
          Vector2f mp = w.mapPixelToCoords(Mouse::getPosition(w));
          if (!solving) {
            if (b_brs.upd(mp, true)) {
              string p = get_file_dialog(0);
              if (!p.empty() && p != "ERROR") {
                file_path = p;
                inp.set(p);
              }
            }
            if (b_ld.upd(mp, true)) {
              ifstream in(file_path);
              if (in) {
                string l;
                grid.clear();
                while (getline(in, l)) {
                  l.erase(remove(l.begin(), l.end(), '\r'), l.end());
                  l.erase(remove(l.begin(), l.end(), ' '), l.end());
                  l.erase(remove(l.begin(), l.end(), '\t'), l.end());
                  if (l.empty()) continue;
                  grid.push_back(l);
                }
                n = grid.size();
                bool valid = (n > 0);
                if (valid) {
                  m = grid[0].length();
                  for (int i = 1; i < n; i++) {
                    if ((int)grid[i].length() != m) {
                      valid = false;
                      break;
                    }
                  }
                }
                if (valid && m > 0) {
                  string err = validate_board();
                  if (err.empty()) {
                    ans.assign(n, -1);
                    found = false;
                    iter = 0;
                    save_msg = "";
                    st.setString("Loaded " + to_string(n) + "x" + to_string(m));
                    st.setFillColor(Color::White);
                  } else {
                    st.setString("Invalid:\n" + err);
                    st.setFillColor(Color::Red);
                    grid.clear();
                    n = 0;
                    m = 0;
                  }
                } else {
                  st.setString("Invalid board:\nRows must have "
                               "equal length");
                  st.setFillColor(Color::Red);
                  grid.clear();
                  n = 0;
                  m = 0;
                }
              } else {
                st.setString("File error");
                st.setFillColor(Color::Red);
              }
            }
            if (b_m1.upd(mp, true)) {
              mode = 1;
              b_m1.a = true;
              b_m2.a = false;
            }
            if (b_m2.upd(mp, true)) {
              mode = 2;
              b_m1.a = false;
              b_m2.a = true;
            }
            if (n > 0 && b_run.upd(mp, true)) {
              found = false;
              iter = 0;
              stop = false;
              solving = true;
              save_msg = "";
              thread(run).detach();
            }
            if (found && b_svt.upd(mp, true)) {
              save_txt();
            }
            if (found && b_sv.upd(mp, true)) {
              save_img();
            }
          }
          if (solving && b_stp.upd(mp, true)) stop = true;
        }
      }
    }
    Vector2f mp = w.mapPixelToCoords(Mouse::getPosition(w));
    sld.upd(w);
    delay_ms = (int)sld.cur;
    if (!solving) {
      b_brs.upd(mp, false);
      b_ld.upd(mp, false);
      b_m1.upd(mp, false);
      b_m2.upd(mp, false);
      b_run.upd(mp, false);
      b_stp.upd(mp, false);
      if (found) b_svt.upd(mp, false);
      if (found) b_sv.upd(mp, false);
    }
    if (solving) {
      st.setString("Iter: " + to_string(iter));
      st.setFillColor(Color::Yellow);
    } else if (save_msg != "") {
      st.setString(save_msg);
      st.setFillColor(Color::Cyan);
      if (save_msg.length() > 35) st.setCharacterSize(10);
    } else if (found) {
      st.setString("SOLVED\nIter: " + to_string(iter) + "\nTime: " + to_string((int64_t)elapsed_time) + "ms");
      st.setFillColor(Color::Green);
      st.setCharacterSize(14);
    } else if (stop) {
      st.setString("STOPPED\nIter: " + to_string(iter) + "\nTime: " + to_string((int64_t)elapsed_time) + "ms");
      st.setFillColor(Color(255, 100, 100));
    } else if (n > 0 && iter > 0) {
      st.setString("FAILED\nIter: " + to_string(iter) + "\nTime: " + to_string((int64_t)elapsed_time) + "ms");
      st.setFillColor(Color::Red);
    }
    w.clear(C_BG);
    w.draw(pn);
    w.draw(tit);
    w.draw(nm);
    w.draw(l_f);
    inp.drw(w);
    b_brs.drw(w);
    b_ld.drw(w);
    w.draw(l_m);
    b_m1.drw(w);
    b_m2.drw(w);
    sld.drw(w);
    b_run.drw(w);
    b_stp.drw(w);
    w.draw(st);
    if (found && !solving) b_svt.drw(w);
    if (found && !solving) b_sv.drw(w);
    draw(w, W_WIDTH - 340.f, W_HEIGHT - 40.f, 320.f, 20.f);
    w.display();
  }
  return 0;
}
