#include "barclasses.h"
#include <math.h>
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

//======================barcode================


bc::Barcode::Barcode()
{
}

bc::Barcode::Barcode(const bc::Barcode& obj)
{
    bar.insert(bar.begin(), obj.bar.begin(), obj.bar.end());
}

void bc::Barcode::add(uchar st, uchar len)
{
    bar.push_back(bline(st, len));
}

int bc::Barcode::sum() const
{
    int sum = 0;
    for (const bline& l : bar)
        sum += l.len;
    return sum;
}

bc::Barcode* bc::Barcode::clone() const
{
    Barcode* nb = new Barcode();
    nb->bar.insert(nb->bar.begin(), bar.begin(), bar.end());
    return nb;
}

uchar bc::Barcode::maxLen() const
{
    uchar max = 0;
    for (const bline& l : bar)
        if (l.len > max)
            max = l.len;
    return max;
}

void bc::Barcode::relen()
{
    if (bar.size() == 0)
        return;
    uchar mini = bar[0].start;
    for (size_t i = 1; i < bar.size(); ++i)
        if (bar[i].start < mini)
            mini = bar[i].start;

    for (size_t i = 0; i < bar.size(); ++i)
        bar[i].start -= mini;

    //mini = std::min_element(arr.begin(), arr.end(), [](bline &b1, bline &b2) { return b1.start < b2.start; })->start;
    //std::for_each(arr.begin(), arr.end(), [mini](bline &n) {return n.start - uchar(mini); });
}

void bc::Barcode::removePorog(const uchar porog)
{
    if (porog == 0)
        return;
    Barcode res;
    for (const bline& line : bar) {
        if (line.len >= porog)
            res.bar.push_back(line);
    }
    bar.clear();
    bar.insert(bar.begin(), res.bar.begin(), res.bar.end());
}

void bc::Barcode::preprocessBar(const int& porog, bool normalize)
{
    if (porog > 0)
        this->removePorog((uchar)roundf((porog * float(this->maxLen()) / 100.f)));

    if (normalize)
        this->relen();
}


float bc::Barcode::compireCTML(const bc::Barbase* bc) const
{
    Barcode* Y = dynamic_cast<const Barcode*>(bc)->clone();
    Barcode* X = clone();
    if (X->bar.size() == 0 || Y->bar.size() == 0)
        return 0;
    float sum = (float)(X->sum() + Y->sum());
    int n = static_cast<int>(MIN(bar.size(), Y->bar.size()));

    float tsum = 0.f;
    for (int re = 0; re < n; ++re) {
        float maxCoof = 0;
        float maxsum = 0;
        int ik = 0;
        int jk = 0;
        for (int i = 0, total = X->bar.size(); i < total; ++i)
        {
            for (int j = 0, total = Y->bar.size(); j < total; ++j)
            {
                short st = MAX(X->bar[i].start, Y->bar[j].start);
                short ed = MIN(X->bar[i].start + X->bar[i].len, Y->bar[j].start + Y->bar[j].len);
                float minlen = (float)(ed - st);
                float maxlen = MAX(X->bar[i].len, Y->bar[j].len);
                //���� ������ 0, ������ ����� �� ������������
                if (minlen <= 0 || maxlen <= 0)
                    continue;

                float coof = minlen / maxlen;
                if (coof > maxCoof)
                {
                    maxCoof = coof;
                    maxsum = (float)(X->bar[i].len + Y->bar[j].len);
                    ik = i;
                    jk = j;
                }
            }
        }
        X->bar.erase(X->bar.begin() + ik);
        Y->bar.erase(Y->bar.begin() + jk);
        tsum += (maxsum / sum) * maxCoof;
    }
    return tsum;
}

float bc::Barcode::compireCTS(const bc::Barbase* bc) const
{
    Barcode* Y = dynamic_cast<const Barcode*>(bc)->clone();
    Barcode* X = clone();
    if (X->bar.size() == 0 || Y->bar.size() == 0)
        return 0;
    float sum = (float)(X->sum() + Y->sum());
    int n = static_cast<int>(MIN(X->bar.size(), Y->bar.size()));

    float tsum = 0.f;
    for (int re = 0; re < n; ++re) {
        float maxCoof = 0;
        float maxsum = 0;
        int ik = 0;
        int jk = 0;
        for (int i = 0, total = X->bar.size(); i < total; ++i)
        {
            for (int j = 0, total2 = Y->bar.size(); j < total2; ++j)
            {
                short st = MAX(X->bar[i].start, Y->bar[j].start);
                short ed = MIN(X->bar[i].start + X->bar[i].len, Y->bar[j].start + Y->bar[j].len);
                float minlen = (float)(ed - st);

                st = MIN(X->bar[i].start, Y->bar[j].start);
                ed = MAX(X->bar[i].start + X->bar[i].len, Y->bar[j].start + Y->bar[j].len);
                float maxlen = (float)(ed - st);

                //���� ������ 0, ������ ����� �� ������������
                if (minlen <= 0 || maxlen <= 0)
                    continue;

                float coof = minlen / maxlen;
                if (coof > maxCoof)
                {
                    maxCoof = coof;
                    maxsum = (float)(X->bar[i].len + Y->bar[j].len);
                    ik = i;
                    jk = j;
                }
            }
        }
        X->bar.erase(X->bar.begin() + ik);
        Y->bar.erase(Y->bar.begin() + jk);
        tsum += (maxsum / sum) * maxCoof;
    }
    return tsum;
}

bc::Barcode::~Barcode()
{
    bar.clear();
}


////=======================barcodeTwo=====================

bc::BarcodeTwo::BarcodeTwo()
{

}

bc::BarcodeTwo::BarcodeTwo(bc::Barbase* one, bc::Barbase* two, bool allocateNewpointers)
{
    init(one, two, allocateNewpointers);
}

bc::BarcodeTwo::BarcodeTwo(const bc::BarcodeTwo& obj)
{
    one = obj.one->clone();
    two = obj.two->clone();
}

void bc::BarcodeTwo::init(bc::Barbase* one, bc::Barbase* two, bool allocateNewpointers)
{
    if (this->one != nullptr)
        delete one;
    if (this->two != nullptr)
        delete two;
    if (allocateNewpointers)
    {
        this->one = one->clone();
        this->two = two->clone();
    }
    else {
        this->one = one;
        this->two = two;
    }
}

bc::BarcodeTwo* bc::BarcodeTwo::clone() const
{
    return new BarcodeTwo(one, two, true);
}

int bc::BarcodeTwo::sum() const
{
    return one->sum() + two->sum();
}

void bc::BarcodeTwo::preprocessBar(const int& porog, bool normalize)
{
    one->preprocessBar(porog, normalize);
    two->preprocessBar(porog, normalize);
}

float bc::BarcodeTwo::compireCTML(const bc::Barbase* bc) const
{
    const BarcodeTwo* b2 = dynamic_cast<const BarcodeTwo*>(bc);
    float r1 = one->compireCTML(b2->one);
    float r2 = two->compireCTML(b2->two);
    int summ1 = one->sum() + b2->one->sum();
    int summ2 = two->sum() + b2->two->sum();
    float procWhite = ((float)summ1 / (summ1 + summ2));
    return r1 * procWhite + r2 * (1 - procWhite);
}

float bc::BarcodeTwo::compireCTS(const Barbase* bc) const
{
    const BarcodeTwo* b2 = dynamic_cast<const BarcodeTwo*>(bc);

    float r1 = one->compireCTS(b2->one);
    float r2 = two->compireCTS(b2->two);
    int summ1 = one->sum() + b2->one->sum();
    int summ2 = two->sum() + b2->two->sum();
    float procWhite = ((float)summ1 / (summ1 + summ2));
    return r1 * procWhite + r2 * (1 - procWhite);
}

void bc::BarcodeTwo::removePorog(const uchar porog)
{
    one->removePorog(porog);
    two->removePorog(porog);
}

bc::BarcodeTwo::~BarcodeTwo()
{
    if (one != nullptr)
    {
        delete one;
        one = nullptr;
    }

    if (two != nullptr)
    {
        delete two;
        two = nullptr;
    }
}

void bc::BarcodeTwo::relen()
{
    one->relen();
    two->relen();
}

//====================================================

bc::Barbase::~Barbase() {}

float bc::Barbase::compireBarcodes(const bc::Barbase* X, const bc::Barbase* Y, const CompireFunction& type)
{
    switch (type) {
    case CompireFunction::CommonToLen:
        return X->compireCTML(Y);
        break;
    case CompireFunction::CommonToSum:
        return X->compireCTS(Y);
        break;
    default:
        return 0;
        //X->compireCTML(Y);
        break;
    }
}
/////////////////////////////////////////////////////////
//=================================barocdeTGB

bc::BarcodeRGB* bc::BarcodeRGB::clone() const
{
    return new BarcodeRGB(barR, barG, barB, true);
}

bc::BarcodeRGB::BarcodeRGB() {}

bc::BarcodeRGB::BarcodeRGB(bc::Barbase* r, bc::Barbase* g, bc::Barbase* b, bool allocateNewpointers)
{
    init(r, g, b, allocateNewpointers);
}

bc::BarcodeRGB::BarcodeRGB(const bc::BarcodeRGB& obj)
{
    this->barR = obj.barR->clone();
    this->barG = obj.barG->clone();
    this->barB = obj.barB->clone();
}

void bc::BarcodeRGB::init(bc::Barbase* r, bc::Barbase* g, bc::Barbase* b, bool allocateNewpointers)
{
    if (barR != nullptr)
        delete barR;

    if (barG != nullptr)
        delete barG;

    if (barB != nullptr)
        delete barB;

    if (allocateNewpointers) {
        this->barR = r->clone();
        this->barG = g->clone();
        this->barB = b->clone();
    }
    else {
        this->barR = r;
        this->barG = g;
        this->barB = b;
    }
}


void bc::BarcodeRGB::preprocessBar(const int& porog, bool normalize)
{
    this->barR->preprocessBar(porog, normalize);
    this->barG->preprocessBar(porog, normalize);
    this->barB->preprocessBar(porog, normalize);
}

float bc::BarcodeRGB::compireCTML(const bc::Barbase* bc) const
{
    const BarcodeRGB* Y = dynamic_cast<const BarcodeRGB*>(bc);
    float r0 = barR->compireCTML(Y->barR);
    float r1 = barG->compireCTML(Y->barG);
    float r2 = barB->compireCTML(Y->barB);
    int sum0 = (barR->sum() + Y->barR->sum());
    int sum1 = (barG->sum() + Y->barG->sum());
    int sum2 = (barB->sum() + Y->barB->sum());
    float tsum = (float)(sum0 + sum1 + sum2);
    return r0 * (sum0 / tsum) + r1 * (sum1 / tsum) + r2 * (sum2 / tsum);
}

float bc::BarcodeRGB::compireCTS(const bc::Barbase* bc) const
{
    const BarcodeRGB* Y = dynamic_cast<const BarcodeRGB*>(bc);
    float r0 = barR->compireCTS(Y->barR);
    float r1 = barG->compireCTS(Y->barG);
    float r2 = barB->compireCTS(Y->barB);
    int sum0 = (barR->sum() + Y->barR->sum());
    int sum1 = (barG->sum() + Y->barG->sum());
    int sum2 = (barB->sum() + Y->barB->sum());
    float tsum = (float)(sum0 + sum1 + sum2);
    return r0 * (sum0 / tsum) + r1 * (sum1 / tsum) + r2 * (sum2 / tsum);
}

void bc::BarcodeRGB::removePorog(const uchar porog)
{
    this->barR->removePorog(porog);
    this->barG->removePorog(porog);
    this->barB->removePorog(porog);
}

bc::BarcodeRGB::~BarcodeRGB()
{
    if (barR != nullptr)
    {

        delete barR;
        barR = nullptr;
    }
    if (barG != nullptr) {
        delete barG;
        barG = nullptr;
    }
    if (barB != nullptr)
    {
        delete barB;
        barB = nullptr;
    }
}

int bc::BarcodeRGB::sum() const
{
    return barR->sum() + barG->sum() + barB->sum();
}

void bc::BarcodeRGB::relen()
{
    barR->relen();
    barG->relen();
    barB->relen();
}


bc::Baritem::Baritem() {}

bc::Baritem::Baritem(const bc::Baritem& obj)
{
    bar.insert(bar.begin(), obj.bar.begin(), obj.bar.end());
}

void bc::Baritem::add(uchar st, uchar len)
{
    bar.push_back(new bline(st, len));
}

//void bc::Baritem::add(uchar st, uchar len, cv::Mat binmat)
//{
//    bar.push_back(bline(st, len, binmat));
//}
void bc::Baritem::add(uchar st, uchar len, pmap* binmat)
{
    bar.push_back(new bline(st, len, binmat));
}

int bc::Baritem::sum() const
{
    int sum = 0;
    for (const bline* l : bar)
        sum += l->len;

    return sum;
}

bc::Baritem* bc::Baritem::clone() const
{
    Baritem* nb = new Baritem();
    nb->bar.insert(nb->bar.begin(), bar.begin(), bar.end());
    return nb;
}

uchar bc::Baritem::maxLen() const
{
    uchar max = 0;
    for (const bline* l : bar)
        if (l->len > max)
            max = l->len;

    return max;
}

void bc::Baritem::relen()
{
    if (bar.size() == 0)
        return;

    uchar mini = bar[0]->start;
    for (size_t i = 1; i < bar.size(); ++i)
        if (bar[i]->start < mini)
            mini = bar[i]->start;

    for (size_t i = 0; i < bar.size(); ++i)
        bar[i]->start -= mini;

    //mini = std::min_element(arr.begin(), arr.end(), [](bline &b1, bline &b2) { return b1.start < b2.start; })->start;
    //std::for_each(arr.begin(), arr.end(), [mini](bline &n) {return n.start - uchar(mini); });
}

void bc::Baritem::removePorog(const uchar porog)
{
    if (porog == 0)
        return;
    Baritem res;
    for (bline* line : bar) {
        if (line->len >= porog)
            res.bar.push_back(line);
        else
            delete line;
    }
    bar.clear();
    bar.insert(bar.begin(), res.bar.begin(), res.bar.end());
}

void bc::Baritem::preprocessBar(const int& porog, bool normalize)
{
    if (porog > 0)
        this->removePorog((uchar)roundf((porog * float(this->maxLen()) / 100.f)));

    if (normalize)
        this->relen();
}


float bc::Baritem::compireCTML(const bc::Barbase* bc) const
{
    Baritem* Y = dynamic_cast<const Baritem*>(bc)->clone();
    Baritem* X = clone();
    if (X->bar.size() == 0 || Y->bar.size() == 0)
        return 0;
    float sum = (float)(X->sum() + Y->sum());
    int n = static_cast<int>(MIN(bar.size(), Y->bar.size()));

    float tsum = 0.f;
    for (int re = 0; re < n; ++re)
    {
        float maxCoof = 0;
        float maxsum = 0;
        int ik = 0;
        int jk = 0;
        for (int i = 0, totalI = X->bar.size(); i < totalI; ++i) {
            for (int j = 0, totalY = Y->bar.size(); j < totalY; ++j) {
                short st = MAX(X->bar[i]->start, Y->bar[j]->start);
                short ed = MIN(X->bar[i]->start + X->bar[i]->len, Y->bar[j]->start + Y->bar[j]->len);
                float minlen = (float)(ed - st);
                float maxlen = MAX(X->bar[i]->len, Y->bar[j]->len);
                //���� ������ 0, ������ ����� �� ������������
                if (minlen <= 0 || maxlen <= 0)
                    continue;

                float coof = minlen / maxlen;
                if (coof > maxCoof) {
                    maxCoof = coof;
                    maxsum = (float)(X->bar[i]->len + Y->bar[j]->len);
                    ik = i;
                    jk = j;
                }
            }
        }
        X->bar.erase(X->bar.begin() + ik);
        Y->bar.erase(Y->bar.begin() + jk);
        tsum += maxsum * maxCoof;
    }
    return tsum / sum;
}

float bc::Baritem::compireCTS(const bc::Barbase* bc) const
{
    Baritem* Y = dynamic_cast<const Baritem*>(bc)->clone();
    Baritem* X = clone();
    if (X->bar.size() == 0 || Y->bar.size() == 0)
        return 0;
    float sum = (float)(X->sum() + Y->sum());
    int n = static_cast<int>(MIN(X->bar.size(), Y->bar.size()));

    float tsum = 0.f;
    for (int re = 0; re < n; ++re)
    {
        float maxCoof = 0;
        float maxsum = 0;
        int ik = 0;
        int jk = 0;
        for (int i = 0, total = X->bar.size(); i < total; ++i) {
            for (int j = 0, total2 = Y->bar.size(); j < total2; ++j) {
                short st = MAX(X->bar[i]->start, Y->bar[j]->start);
                short ed = MIN(X->bar[i]->start + X->bar[i]->len, Y->bar[j]->start + Y->bar[j]->len);
                float minlen = (float)(ed - st);

                st = MIN(X->bar[i]->start, Y->bar[j]->start);
                ed = MAX(X->bar[i]->start + X->bar[i]->len, Y->bar[j]->start + Y->bar[j]->len);
                float maxlen = (float)(ed - st);

                //���� ������ 0, ������ ����� �� ������������
                if (minlen <= 0 || maxlen <= 0)
                    continue;

                float coof = minlen / maxlen;
                if (coof > maxCoof)
                {
                    maxCoof = coof;
                    maxsum = (float)(X->bar[i]->len + Y->bar[j]->len);
                    ik = i;
                    jk = j;
                }
            }
        }
        X->bar.erase(X->bar.begin() + ik);
        Y->bar.erase(Y->bar.begin() + jk);
        tsum += maxsum * maxCoof;
    }
    return tsum / sum;
}

bc::Baritem::~Baritem()
{
    bar.clear();
}

//=======================barcontainer=====================

bc::Barcontainer::Barcontainer()
{
}

int bc::Barcontainer::sum() const
{
    int sm = 0;
    for (const Baritem* it : items)
        sm += it->sum();

    return sm;
}

void bc::Barcontainer::relen()
{
    for (Baritem* it : items)
        it->relen();
}


uchar bc::Barcontainer::maxLen() const
{
    uchar mx = 0;
    for (const Baritem* it : items) {
        uchar curm = it->maxLen();
        if (curm > mx)
            mx = curm;
    }

    return mx;
}


size_t bc::Barcontainer::count()
{
    return items.size();
}

bc::Baritem* bc::Barcontainer::get(int i)
{
    if (items.size() == 0)
        return nullptr;

    while (i < 0)
        i += items.size();

    while (i >= (int)items.size())
        i -= items.size();

    return items[i];
}

//bc::Baritem *bc::Barcontainer::operator[](int i)
//{
//    if (items.size() == 0)
//        return nullptr;

//    while (i < 0)
//        i += items.size();

//    while (i >= (int) items.size())
//        i -= items.size();

//    return items[i];
//}

bc::Baritem* bc::Barcontainer::lastItem()
{
    if (items.size() == 0)
        return nullptr;

    return items[items.size() - 1];
}

void bc::Barcontainer::addItem(bc::Baritem* item)
{
    items.push_back(item);
}

void bc::Barcontainer::removePorog(const uchar porog)
{
    for (Baritem* it : items)
        it->removePorog(porog);
}

void bc::Barcontainer::preprocessBar(const int& porog, bool normalize)
{
    for (Baritem* it : items)
        it->preprocessBar(porog, normalize);
}

bc::Barbase* bc::Barcontainer::clone() const
{
    Barcontainer* newBar = new Barcontainer();

    for (Baritem* it : items)
        newBar->items.push_back(new Baritem(*it));

    return newBar;
}


float bc::Barcontainer::compireCTML(const bc::Barbase* bc) const
{
    const Barcontainer* bcr = dynamic_cast<const Barcontainer*>(bc);
    float res = 0;
    float s = sum() + bcr->sum();
    for (size_t i = 0; i < MIN(items.size(), bcr->items.size()); i++)
    {
        res += items[i]->compireCTML(bcr->items[i]) * (items[i]->sum() + bcr->items[i]->sum()) / s;
    }

    return res;
}

float bc::Barcontainer::compireBest(const bc::Barbase* bc) const
{
    float res = 0;
    for (Baritem* it : items)
        res = MAX(res, bc->compireCTS(it));

    return res;
}

float bc::Barcontainer::compireCTS(const bc::Barbase* bc) const
{
    float res = 0;
    float s = sum();
    for (Baritem* it : items)
        res += bc->compireCTS(it) * it->sum() / s;

    return res;
}

bc::Barcontainer::~Barcontainer()
{
    for (size_t i = 0; i < items.size(); ++i)
        delete items[i];
}

